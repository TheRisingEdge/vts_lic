#include "PTracker.h"
#include "Draw.h"
#include "Helper.h"
#include <algorithm>
#include "Tool.h"

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

			//////////////////////////////////////////////////////////////debug draw
			auto detections = detectionBuffer[1];
			auto currentFrame = frameBuffer[1];
			auto prevFrame = frameBuffer[0];
			auto currentForeground = foregroundBuffer[1];			

			//filter detections
			//keep only continuous detections
			//look ahead couple of frames to see if the detections persist
			//for now Hog is stable, so we have none false positive detections

			//tracker region; the hard stuff			
			if(tracks.size() == 0 && detections.size() > 0)
			{
				tracks.clear();
				tracks.reserve(detections.size());
				for_each(begin(detections), end(detections), [&](const detection& d){
					track t = {idGenerator.nextId(), d.rect};
					tracks.push_back(t);
				});
				continue;
			}


			//should first propagate tracks via lucas kanade filter
			//track the prev tracks using short term lucas kanade (choose points using fb and NCC)
			//merge detection(if exists) with short term tracker, thus providing measurement for Kalman filter
			//if short term tracker is unreliable then use Kalman to track

			auto it = tracks.begin();
			auto end = tracks.end();
			for(; it != end; ++it)
			{
				trackLK(*(it), frameBuffer, grayFrameBuffer, foregroundBuffer);
			}

			std::map<int, vector<int>> trMatches; 
			std::map<int, vector<int>> detMatches;

			for_each(begin(tracks), std::end(tracks), [&](track& tr){
				int minArea = 0.6 * tr.rect.area();

				auto dit = detections.begin();
				auto dend = detections.end();
				for(;dit != dend; ++dit)
				{
					Rect intersection = tr.rect & dit->rect;
					if(intersection.area() > minArea)
					{												
						//tr.rect = dit->rect;
						trMatches[tr.id].push_back(dit->id);
						detMatches[dit->id].push_back(tr.id);

						//imshow("matches", Helper::concatImages(prevFrame(tr.rect), currentFrame(dit->rect)));
						//cv::waitKey();
					}						
				}					
			});

			for_each(begin(detections), std::end(detections), [&](detection& d){
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
			fclone.release();

			fclone = currentFrame.clone();
			Helper::drawTracks(tracks, fclone);
			imshow("tracks", fclone);
			fclone.release();
			waitKey();


			//for_each(begin(tracks), end(tracks), [&](track& tr){
			//	imshow("track", currentFrame(tr.rect));			
			//	auto matches = trMatches[tr.id];
			//	if(matches.size() > 0)
			//	{
			//		
			//		imshow("detection", currentFrame(detections[matches[0]].rect));
			//	}

			//	cv::waitKey();
			//});

			fclone.release();
		}

		//imshow("frame", frame);
		cv::waitKey(frameDelay);
	
	}
}

track PTracker::initTrackFromDetection( const detection& d,IdGenerator& gen )
{
	track t = {gen.nextId(), d.rect};
	return t;
}

const int XCount = 10;
const int YCount = 10;

void PTracker::trackLK( track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds )
{

	Mat currentFrame = frames[0];
	Mat nextFrame = frames[1];
	Mat currentForeground = foregrounds[0];
	Mat trackForeground = currentForeground(tr.rect);

	int xoffset = tr.rect.width / XCount;
	int yoffset = tr.rect.height/ YCount;
	Point2f tl = Point2f(tr.rect.tl().x,tr.rect.tl().y);
	vector<Point2f> trackedPoints; 
	for(int i = 0; i < XCount; i++)
	{
		for(int j = 0; j < YCount; j++)
		{
			int line = xoffset*i;
			int col = yoffset*j;
			uchar value = trackForeground.at<uchar>(line,col);
			if( value == 255)
				trackedPoints.push_back(tl + Point2f(col,line));			
		}
	}

	
	vector<Mat> gfr;
	gfr.assign(grayFrames.begin(), grayFrames.begin()+1);
	
	auto fresult = forewardTrack(trackedPoints, gfr);

	reverse(gfr.begin(), gfr.end());
	auto bresult = forewardTrack(fresult, gfr);

	auto inliers = filterInliers(trackedPoints, bresult, 0.6);
	
	vector<uchar> status;
	vector<float> errors;
	vector<Point2f> tracked_t;
	cv::calcOpticalFlowPyrLK(
		grayFrames[0],			
		grayFrames[1], // 2 consecutive images, must be gray
		inliers, // input point position in first image
		tracked_t,// output point postion in the second image
		status,	// tracking success
		errors	// tracking error  	   
	);  
	
	int size = inliers.size();
	float* xdiff = new float[size];
	float* ydiff = new float[size];
	for(int i = 0; i < size; i++)
	{
		xdiff[i] =  tracked_t[i].x - inliers[i].x;
		ydiff[i] = tracked_t[i].y - inliers[i].y;	
	}

	float xmedian = Tool::median(xdiff, size);
	float ymedian = Tool::median(ydiff, size);
	delete[] xdiff;
	delete[] ydiff;

	auto fclone = currentFrame.clone();
	Helper::drawFPoints(inliers, fclone);
	imshow("inliers", fclone);

	fclone = currentFrame.clone();
	Helper::drawRect(tr.rect, fclone);
	//printf("track from detector\n");
	//imshow("track", fclone);
	fclone.release();
	//cv::waitKey();

	fclone = currentFrame.clone();
	Tool::moveRect(tr.rect, xmedian, ymedian);
	Draw::rect(tr.rect, fclone);
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