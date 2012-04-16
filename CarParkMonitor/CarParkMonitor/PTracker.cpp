#include "PTracker.h"
#include "Draw.h"
#include "Helper.h"
#include <algorithm>
#include "Tool.h"

struct trackMatch
{
	int detectionId;
	int intersectionArea;
};

bool byArea(trackMatch a, trackMatch b)
{
	return a.intersectionArea > b.intersectionArea;
}

void PTracker::start()
{
	long trainingFrames = 100;
	int fps;
	double frameDelay;
	int frameCount = 0;

	if(!video.openCapture(fps, frameDelay))
		return;

	IdGenerator idGenerator(0);

	int bufferSize = 10;
	vector<track> tracks;
	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<detection>> detectionBuffer;

	Mat frame;
	while(video.read(frame))
	{
		frameCount++;
		if(video.frameCount < trainingFrames)
		{
			subtractor->learn(frame);
		}else{
			Mat foreground = subtractor->segment(frame);
			ClassifierParams cparam = {frame};
			auto frameDetections = classifier->detect(cparam);

			//////////////////////////////////////////////////////////////
			frameBuffer.push_back(frame.clone());
			foregroundBuffer.push_back(foreground.clone());
			detectionBuffer.push_back(frameDetections);

			Mat grayFrame;
			cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
			grayFrameBuffer.push_back(grayFrame);

			if(frameBuffer.size() > bufferSize)
			{				
				frameBuffer[0].release(); 
				foregroundBuffer[0].release();
				grayFrameBuffer[0].release();
				detectionBuffer[0].clear();

				frameBuffer.erase(frameBuffer.begin());
				grayFrameBuffer.erase(grayFrameBuffer.begin());
				foregroundBuffer.erase(foregroundBuffer.begin());
				detectionBuffer.erase(detectionBuffer.begin());
			}else
				continue;

			////////////////////////////////////////////////////////////////debug draw
			auto detections = detectionBuffer[1];
			auto currentFrame = frameBuffer[1];
			auto prevFrame = frameBuffer[0];
			auto currentForeground = foregroundBuffer[1];			

			if(tracks.size() == 0 && detections.size() > 0)
			{
				tracks.clear();
				tracks.reserve(detections.size());
				for_each(begin(detections), end(detections), [&](const detection& d){
					track t = initTrackFromDetection(d , idGenerator);
					tracks.push_back(t);
				});
				continue;
			}

			auto it = tracks.begin();
			auto tend = tracks.end();
			for(; it != tend; ++it)
				trackLK(*(it), frameBuffer, grayFrameBuffer, foregroundBuffer);


			std::map<int, trackMatch> trMatches; 
			std::map<int, int> detMatches;

			for_each(begin(tracks), end(tracks), [&](track& tr){
				int minArea = 0.5 * tr.asRecti().area();

				auto dit = detections.begin();
				auto dend = detections.end();
				for(;dit != dend; ++dit)
				{
					Rect intersection = tr.asRecti() & dit->rect;
					int area = intersection.area();

					if(area > minArea)
					{	
						trackMatch m = {dit->id, area};
						if(trMatches.find(tr.id) == trMatches.end())											
						{
							tr.assign(dit->rect);
							trMatches[tr.id] = m;
							detMatches[dit->id] = tr.id;							
						}else if(trMatches[tr.id].intersectionArea < area )
						{
							tr.assign(dit->rect);
							trMatches[tr.id] = m;
							detMatches[dit->id] = tr.id;
						}																														
					}						
				}
			});

			for_each(begin(detections), end(detections), [&](detection& d){
				auto it = detMatches.find(d.id);
				if(it == detMatches.end())
				{			
					auto track = initTrackFromDetection(d, idGenerator);
					tracks.push_back(track);
				}
			});

			auto fclone = currentFrame.clone();
			Helper::drawDetections(detections, fclone);
			imshow("detections", fclone);

			Helper::drawTracks(tracks, fclone, Scalar(255,0,0));
			imshow("tracks", fclone);
			fclone.release();
			waitKey();

			fclone.release();
		}

		cv::waitKey(frameDelay);	
	}
}

track PTracker::initTrackFromDetection( const detection& d,IdGenerator& gen )
{
	Point tl = d.rect.tl();
	auto frect = Rect_<float>(tl.x, tl.y, d.rect.width, d.rect.height);
	track t = { gen.nextId(), frect };
	return t;
}

const int XCount = 10;
const int YCount = 10;
const float treshold = 0.6;

void PTracker::trackLK( track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds )
{
	Mat currentFrame = frames[0];
	Mat nextFrame = frames[1];
	Mat currentForeground = foregrounds[0];
	Mat trackForeground = currentForeground(tr.rectf);

	int xoffset = tr.rectf.width / XCount;
	int yoffset = tr.rectf.height/ YCount;
	Point tl = tr.rectf.tl();

	vector<Point2f> trackedPoints; 
	for(int i = 0; i < XCount; i++)
	{
		for(int j = 0; j < YCount; j++)
		{
			int line = xoffset*i;
			int col = yoffset*j;
			uchar value = trackForeground.at<uchar>(line,col);
			if( value == 255)
				trackedPoints.push_back(Point2f(tl.x, tl.y) + Point2f(col,line));			
		}
	}

	if(trackedPoints.size() < 5)
		return;

	vector<Point2f> inliers;
	vector<Point2f> ftracked;
	vector<Point2f> btracked;
	vector<uchar> fstatus, bstatus;
	vector<float> ferrors, berrors;

	cv::calcOpticalFlowPyrLK(
		grayFrames[0],			
		grayFrames[1], 
		trackedPoints, 
		ftracked,
		fstatus,
		ferrors	
	);

	cv::calcOpticalFlowPyrLK(
		grayFrames[1],			
		grayFrames[0],
		ftracked,
		btracked,
		bstatus,
		berrors
	);

	int size = trackedPoints.size();
	float* xdiffs = new float[size];
	float* ydiffs = new float[size];
	int successCount = 0;

	for(int i = 0; i < size; i++)
	{			
		if(fstatus[i] == 1 && bstatus[i] == 1)
		{
			auto vec = btracked[i] - trackedPoints[i];
			float dist = vec.x * vec.x + vec.y * vec.y;		
			if(dist < treshold)
			{
				inliers.push_back(trackedPoints[i]);
				Point2f diff = ftracked[i] - trackedPoints[i];
				xdiffs[successCount] = diff.x;
				ydiffs[successCount] = diff.y;
				successCount++;
			}			
		}
	}

	float xmedian = Tool::median(xdiffs, successCount);
	float ymedian = Tool::median(ydiffs, successCount);
	delete[] xdiffs;
	delete[] ydiffs;

	float *scales = new float[size*(size-1)/2];
	int comparisons = 0;
	for (int i = 0; i < size - 1; i++) {
		for (int j = i + 1; j < size; j++) {
			if (fstatus[i] == 1 && fstatus[j] == 1) {

				float dxPrev = trackedPoints[j].x - trackedPoints[i].x;
				float dyPrev = trackedPoints[j].y - trackedPoints[i].y;

				float dxNext = ftracked[j].x - ftracked[i].x;
				float dyNext = ftracked[j].y - ftracked[i].y;

				float sPrev = sqrt(dxPrev * dxPrev + dyPrev * dyPrev);
				float sNext = sqrt(dxNext * dxNext + dyNext * dyNext);

				if (sPrev != 0 && sNext != 0) {					
					scales[comparisons] = sNext / sPrev;
					comparisons++;
				}
			}
		}
	}

	float scale = Tool::median(scales, comparisons);
	assert(scale > 0.0001);

	auto fclone = currentFrame.clone();
	//Helper::drawFPoints(inliers, fclone);
	//imshow("inliers", fclone);
	//fclone.release();
	//cv::waitKey();

	fclone = currentFrame.clone();
	/*Helper::drawRect(tr.rect, fclone);
	printf("track from detector\n");
	imshow("track", fclone);*/
	fclone.release();
	//cv::waitKey();

	fclone = currentFrame.clone();
	Tool::moveRectf(tr.rectf, xmedian, ymedian);
	//Tool::scaleRectf(tr.rectf, scale, scale);
	//Draw::rectf(tr.rectf, fclone);
	//printf("track from LK\n");
	//imshow("track", fclone);
	//cv::waitKey();
	fclone.release();
}

vector<Point2f> PTracker::forewardTrack( vector<Point2f> points, vector<Mat> grayFrames)
{
	assert(grayFrames.size() > 1);
	assert(points.size() > 0);

	int count = grayFrames.size() - 1;
	
	vector<Point2f> points_t = points;
	vector<Point2f> points_t1;
	vector<uchar> status;
	vector<float> errors;
	for(int i = 0; i < count ; i++)
	{
		points_t1.clear();

		cv::calcOpticalFlowPyrLK(
			grayFrames[i],			
			grayFrames[i+1], // 2 consecutive images, must be gray
			points_t, // input point position in first image
			points_t1,// output point postion in the second image
			status,	// tracking success
			errors	// tracking error  	   
		);  

		if(secondFrameTrackedPoints.size() == 0)
			secondFrameTrackedPoints.assign(points_t1.begin(), points_t1.end());

		status.clear();
		errors.clear();
		points_t = points_t1;


/*		Mat c2 = grayFrames[i+1].clone();		
		Helper::drawFPoints(points_t1, c2);
	
		imshow("keypoints", c2);
		printf("frame %d\n", i+1);

		cv::waitKey();
		c2.release();*/		
	}
	//printf("\n");

	return points_t1;
}

vector<Point2f> PTracker::filterInliers( const vector<Point2f>& forewardPoints,const vector<Point2f>& backPoints, float treshold)
{
	assert(forewardPoints.size() == backPoints.size());
	vector<Point2f> result;	

	int size = forewardPoints.size();
	for(int i = 0; i < size; i++)
	{
		Point2f pf = forewardPoints[i];
		Point2f pb = backPoints[i];

		float xdif = abs(pf.x - pb.x);
		float ydif = abs(pf.y - pb.y);

		float dist = xdif*xdif + ydif*ydif;
		if( dist < treshold)		
			result.push_back(pf);					
	}
	return result;
}