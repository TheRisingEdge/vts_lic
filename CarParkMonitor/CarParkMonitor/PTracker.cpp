#include "PTracker.h"
#include "Draw.h"
#include "Helper.h"
#include <algorithm>
#include "Tool.h"
#include "BlobDetector.h"
#include "lbp.h"
#include "histogram.h"

struct trackMatch
{
	int detectionId;
	int intersectionArea;
	float score;
};

struct detectionMatch
{
	int trackId;
	float score;
};

static const int bufferSize = 3;

bool PTracker::shiftBuffers( Mat frame )
{

	Mat grayFrame;
	cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

	Mat foreground = subtractor->segment(frame);

	ClassifierParams cparam = {frame};
	auto frameDetections = classifier->detect(cparam);

	frameBuffer.push_back(frame.clone());
	foregroundBuffer.push_back(foreground.clone());
	detectionBuffer.push_back(frameDetections);	
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
		return false;

	return true;
}

void PTracker::start()
{
	long trainingFrames = 100;
	int fps;
	double frameDelay;
	

	if(!video.openCapture(fps, frameDelay))
		return;

	IdGenerator idGenerator(0);
		
	Mat frame;
	while(video.read(frame))
	{				
		frameCount = video.frameCount;
		frameRect = Rect(0,0, frame.cols, frame.rows);
		if(video.frameCount < trainingFrames)
		{
			subtractor->learn(frame);
		}else{
			
			bool buffersFull = shiftBuffers(frame);
			if(!buffersFull)
				continue;
	
#pragma region getting_frames
			auto detections = detectionBuffer[1];
			auto currentFrame = frameBuffer[1];
			auto currentGrayFrame = grayFrameBuffer[1];
			auto prevFrame = frameBuffer[0];
			auto currentForeground = foregroundBuffer[1];			
#pragma endregion 

#pragma region fist_init_from_detections
			if(tracks.size() == 0 && detections.size() > 0)
			{
				tracks.clear();
				tracks.reserve(detections.size());
				for_each(begin(detections), end(detections), [&](detection& d){
					initializeTrack(d, idGenerator, grayFrameBuffer[1]);
				});
				continue;
			}
#pragma endregion

#pragma region prediction_tracks_positions
			vector<track> tracksToErase;
			auto lkoutput = currentFrame.clone();

			auto it = tracks.begin();
			auto tend = tracks.end();
			for(; it != tend; ++it)
			{				
				if(trackHasExited(*it, frame))				
				{
					tracksToErase.push_back(*it);
					continue;
				}
					
				Rect lkRect, kalmanRect;
				bool lkSuccess = trackLucasKanade(*it, frameBuffer, grayFrameBuffer, foregroundBuffer,lkRect,lkoutput);				
				bool kalmanSuccess = predictKalman(*it, kalmanRect);
				mergePredictions(lkSuccess, kalmanSuccess ,*it, lkRect, kalmanRect, grayFrameBuffer);
			}
			
			for_each(begin(tracksToErase), end(tracksToErase), [&](track tr){
				deleteTrack(tr);
			});	
#pragma endregion

			std::map<int, trackMatch> trMatches; 
			std::map<int, detectionMatch> detMatches;
			std::map<int, Mat> detectionHists;

			if(video.frameCount == 273)
			{int a; a = 3;}

			matcher->begin();
			secMatcher->begin();
			for_each(begin(tracks), end(tracks), [&](track& tr){													
				auto dit = detections.begin();
				auto dend = detections.end();
				for(;dit != dend; ++dit)
				{
					float score = matcher->match(tr, *dit, currentGrayFrame);
					if(score < 0)
					continue;
					
					
					//if(detectionHists.find(dit->id) == detectionHists.end())
					//{
					//	auto detHist = lbpHist(currentGrayFrame(dit->rect));
					//	detectionHists[dit->id] = detHist;
					//}

					//auto dhist = detectionHists[dit->id];
					//double dist = lbp::chi_square(tr.model.lbpHist, dhist);

					float dist = secMatcher->match(tr, *dit, currentGrayFrame);
					if(dist > 2500)
					{
						cv::waitKey();
						continue;
					}
						
					
					//imshow("track", tr.model.elbp);
					//imshow("det", currentFrame(dit->rect));
					//printf("%g\n", dist);
					////cv::waitKey();

					trackMatch trMatch = {dit->id, 0, score};
					detectionMatch dMatch = {tr.id, score};

					if(trMatches.find(tr.id) == trMatches.end())
					{
						trMatches[tr.id] = trMatch;
						detMatches[dit->id] = dMatch;																	
						
					}else if(trMatches[tr.id].score < score)
					{			
						auto prevTMatch = detMatches[dit->id];						
						trMatches.erase(prevTMatch.trackId);

						trMatches[tr.id] = trMatch;
						detMatches[dit->id] = dMatch;												
					}
				}							
			});			

#pragma region update_tracks_and_kalman_from_detections
			for_each(begin(tracks), end(tracks), [&](track& tr){
				if(trMatches.find(tr.id) != trMatches.end())//matched detection with track
				{
					auto m = trMatches[tr.id];
					matcher->inferModel(tr, detections[m.detectionId], currentGrayFrame);
					secMatcher->inferModel(tr, detections[m.detectionId], currentGrayFrame);
					validators[tr.id].tick(true);

					correctKalman(tr);					
				}else
				{//detection not found for track
					validators[tr.id].tick(false);
					forwardKalman(tr);
				}
			});
#pragma endregion

#pragma region track_initiation_from_detections
			for_each(begin(detections), end(detections), [&](detection& mockdet){
				auto it = detMatches.find(mockdet.id);
				if(it == detMatches.end()){				
					auto inited = initializeTrack(mockdet, idGenerator, currentGrayFrame);
					trackMatch m = {mockdet.id, 1, 1};
					trMatches[inited.id] = m;					
				}
			});
#pragma endregion 

#pragma region drawing_results
			auto fclone = currentFrame.clone();			
			 
			//imshow("foreground", currentForeground);
			//imshow("lk", lkoutput);

			
			Helper::drawDetections(detections, fclone);
			//imshow("detections", fclone);			
			//		
			Helper::drawTracks(tracks, fclone, Scalar(255,0,0));
			//imshow("tracks", fclone);

			for_each(begin(tracks), end(tracks), [&](track& tr){
				Helper::drawRect(tr.model.kalmanRect, fclone, Scalar(0,0,255));
			});

			imshow("kalman", fclone);
			
			//if(frameCount == 272)
				//waitKey();

			fclone.release();
			lkoutput.release();
#pragma endregion
		}

		cv::waitKey(frameDelay);	
	}
}

track PTracker::createFromDetection( detection& d,IdGenerator& gen, Mat& frame)
{
	Point tl = d.rect.tl();
	auto rectf = Rect_<float>(tl.x, tl.y, d.rect.width, d.rect.height);
	track t = { gen.nextId(), rectf };
	
	secMatcher->inferModel(t, d, frame);
	return t;
}


#pragma region lucasKanade_Kalman_predictors
const int XCount = 15;
const int YCount = 15;
const float treshold = 0.3;
bool PTracker::trackLucasKanade( track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds,Rect& predictedRect ,Mat& output )
{	
	auto trackRect = tr.asRecti();
	Mat currentForeground = foregrounds[0];
	
	if(trackRect.area() < 10 || !Tool::rectInside(trackRect, Rect(0,0, currentForeground.cols, currentForeground.rows)))
		return false;

	Mat trackForeground = currentForeground(trackRect);
	Mat currentFrame = frames[0];
	Mat nextFrame = frames[1];

	int xoffset = trackRect.width / XCount;
	int yoffset = trackRect.height/ YCount;
	Point tl = trackRect.tl();

	vector<Point2f> trackedPoints; 
	for(int i = 0; i < XCount; i++)
	{
		for(int j = 0; j < YCount; j++)
		{
			int line = xoffset*i;
			int col = yoffset*j;
			
			if(line >= trackForeground.rows || col >= trackForeground.cols)
				continue;

			uchar value = trackForeground.at<uchar>(line,col);
			if( value == 255)
				trackedPoints.push_back(Point2f(tl.x, tl.y) + Point2f(col,line));			
		}
	}

	if(trackedPoints.size() < 5)
		return false;

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
			float dist = sqrt(vec.x * vec.x + vec.y * vec.y);		
			if(dist < treshold)
			{
				inliers.push_back(trackedPoints[i]);
				Point2f diff = ftracked[i] - trackedPoints[i];
				xdiffs[successCount] = diff.x;
				ydiffs[successCount] = diff.y;
				successCount++;
			}else
			{
				fstatus[i] = 0;			
				bstatus[i] = 0;
			}				
		}
	}

	if(inliers.size() < 5)
		return false;

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
	//assert(scale > 0.0001);

	//auto fclone = currentFrame.clone();
	//Helper::drawFPoints(inliers, fclone);
	//Helper::drawFPoints(inliers, output);
	//imshow("inliers", fclone);
	//fclone.release();


	//vector<Point2f> ff;
	//for(int i = 0; i < ftracked.size(); i++)
	//if(fstatus[i] == 1)
	//	ff.push_back(ftracked[i]);

	//fclone = nextFrame.clone();
	//Helper::drawFPoints(ff, fclone);
	//imshow("next frame inliers", fclone);
	//fclone.release();

	predictedRect = tr.asRecti();
	Tool::moveRect(predictedRect, xmedian, ymedian);
	Tool::scaleRect(predictedRect, scale, scale);
	predictedRect = frameRect & predictedRect;
	return true;
}

bool PTracker::predictKalman( track& tr, Rect& predictedRect)
{
	if(kalmanFilters.find(tr.id) != kalmanFilters.end())
	{
		auto filter = kalmanFilters[tr.id];		
		auto kalmanResult = filter->predict();
		auto rect = kalmanResult.asRect();		
		predictedRect = rect & frameRect;

		tr.model.kalmanRect = rect;//to be removed		
	}else
		assert(false);

	return true;
}

void PTracker::correctKalman( track& tr )
{
	if(kalmanFilters.find(tr.id) != kalmanFilters.end())
	{
		auto filter = kalmanFilters[tr.id];
		KalmanInput2D input = {tr.asRecti()};
		auto result = filter->correct(input);		
		
		auto rect = result.asRect();
		tr.model.kalmanRect = rect;

	}else
		//this should not happen
		assert(false);	
}

void PTracker::forwardKalman( track& tr )
{
	if(kalmanFilters.find(tr.id) != kalmanFilters.end())
	{
		auto filter = kalmanFilters[tr.id];		
		auto lastPrediction = filter->lastState;

		int vx = floor(lastPrediction.vx);
		int vy = floor(lastPrediction.vy);

		auto lastRect = lastPrediction.asRect();
		Rect fakeRect = lastRect;//Rect(lastRect.x + vx, lastRect.y + vy, lastRect.width, lastRect.height);
		KalmanInput2D fakeInput = {fakeRect};
		auto fakeResult = filter->correct(fakeInput); 
		
		tr.model.kalmanRect = fakeResult.asRect();
	}else
		//this should not happen
		assert(false);	
}
#pragma endregion


track PTracker::initializeTrack( detection& det, IdGenerator& idGenerator, Mat& grayFrame)
{
	auto track = createFromDetection(det, idGenerator, grayFrame);
	tracks.push_back(track);

	auto filter = shared_ptr<KalmanFilter2D>(new KalmanFilter2D());
	KalmanInput2D input = {det.rect};
	filter->init(input);
	kalmanFilters[track.id] = filter;

	Validator v = Validator();
	validators[track.id] = v;
	validators[track.id].tick(true);
	return track;
}

void PTracker::deleteTrack( track& tr )
{
	auto it = tracks.begin();
	auto end = tracks.end();

	for(;it!=end; ++it)
		if(it->id == tr.id){
			kalmanFilters.erase(it->id);
			tracks.erase(it);
			break;
		}

	if(!validators[tr.id].isLost() && validators[tr.id].score() > validators[tr.id].minScore)
	{
		carCount++;		
	}

	validators.erase(tr.id);
}

bool PTracker::trackHasExited( track& tr, Mat frame )
{
	Rect frameRect = Rect(0,0, frame.cols, frame.rows);
	return (!Tool::rectInside(tr.asRecti(), frameRect))||(validators[tr.id].isLost());
}

void PTracker::mergePredictions(bool lkSuccess, bool kalmanSuccess, track& tr, Rect& lkRect, Rect& kalmanRect, vector<Mat> grayBuffer )
{
	auto grayFrame = grayBuffer[1];	
	if(lkSuccess)//compare both
	{
		Mat lkSrc = grayFrame(lkRect);
		Mat kalmanSrc = grayFrame(kalmanRect);				

		float dist0 = secMatcher->distance(tr, lkSrc);
		float dist1 = secMatcher->distance(tr, kalmanSrc);

		if(dist0 < dist1)
		{
			tr.assign(lkRect);			
		}else
		{
			tr.assign(kalmanRect);			
		}				

		//imshow("lk-match",lkSrc);
		//imshow("kalman-match", kalmanSrc);
		//cv::waitKey();
	}		
	else
	{
		tr.assign(kalmanRect);
	}
		
}