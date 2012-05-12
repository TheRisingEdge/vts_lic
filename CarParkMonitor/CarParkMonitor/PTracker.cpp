#include "PTracker.h"
#include "Draw.h"
#include "Helper.h"
#include <algorithm>
#include "Tool.h"
#include "BlobDetector.h"
#include "lbp.h"
#include "histogram.h"

#define PROCESS 1

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

bool PTracker::shiftBuffers( SubFrame subResult, ClasifierFrame classifierResult )
{
	Mat frame = subResult.frame;	
	Mat foreground = subResult.foreground;
	frameRect = Rect(0,0, frame.cols, frame.rows);

	Mat grayFrame;
	cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
	
	auto frameDetections = classifierResult.detections;

	frameBuffer.push_back(frame);
	foregroundBuffer.push_back(foreground);
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

track PTracker::createFromDetection( detection& d,IdGenerator& gen, Mat& frame)
{
	Point tl = d.rect.tl();
	auto rectf = Rect_<float>(tl.x, tl.y, d.rect.width, d.rect.height);
	track t = { gen.nextId(), rectf };
	
	secMatcher->inferModel(t, d, frame);
	return t;
}

#pragma region lucasKanade_Kalman_predictors

const int XCount = 9;
const int YCount = 9;
const int minTrackedPoints = 10;
const float treshold = 0.3;
bool PTracker::trackLucasKanade( track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds,Rect& predictedRect ,Mat& output )
{	
	auto trackRect = tr.asRecti();
	Mat currentForeground = foregrounds[0];
	
	if(trackRect.area() < 10 || !Tool::rectInside(trackRect, Rect(0,0, currentForeground.cols, currentForeground.rows)))
		return false;

	Mat trackForeground = currentForeground(trackRect);

	int xoffset = trackRect.width / XCount;
	int yoffset = trackRect.height / YCount;
	Point tl = trackRect.tl();

	vector<Point2f> trackedPoints; 
	for(int i = 0; i < XCount; i++)
	{
		for(int j = 0; j < YCount; j++)
		{
			int line = yoffset*j;
			int col = xoffset*i;						
			uchar value = trackForeground.at<uchar>(line,col);
			if( value == 255)
				trackedPoints.push_back(Point2f(tl.x, tl.y) + Point2f(col,line));			
		}
	}

	if(trackedPoints.size() < minTrackedPoints)
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

	if(inliers.size() < minTrackedPoints)
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
		assert(false);	
}

void PTracker::forwardKalman( track& tr )
{
	if(kalmanFilters.find(tr.id) != kalmanFilters.end())
	{
		auto filter = kalmanFilters[tr.id];		
		auto lastState = filter->lastState;

		int vx = floor(lastState.vx);
		int vy = floor(lastState.vy);

		auto lastRect = lastState.asRect();
		Rect fakeRect = lastRect;
		Tool::moveRect(fakeRect, vx, vy);

		KalmanInput2D fakeInput = {fakeRect};
		auto fakeResult = filter->correct(fakeInput); 
		
		tr.model.kalmanRect = fakeResult.asRect();
	}else	
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

		if(dist0 <= dist1)		
			tr.assign(lkRect);			
		else		
			tr.assign(kalmanRect);									
	}		
	else
	{
		tr.assign(kalmanRect);
	}		
}

void PTracker::run()
{
	IdGenerator idGenerator(0);

	while(true)
	{
		SubFrame subResult = receive(subtractorBuffer);
		ClasifierFrame classifierResult = receive(classifierBuffer);
	
		//imshow("frame", subResult.frame);
		//imshow("fg", subResult.foreground);
		//waitKey();

		bool buffersFull = shiftBuffers(subResult, classifierResult);
		if(!buffersFull)
			continue;
		
#if PROCESS

#pragma region setting data for time t
		auto detections = detectionBuffer[1];
		auto currentFrame = frameBuffer[1];
		auto currentGrayFrame = grayFrameBuffer[1];
		auto prevFrame = frameBuffer[0];
		auto currentForeground = foregroundBuffer[1];			
#pragma endregion 


#pragma region init tracks from first detections
		if(tracks.size() == 0 && detections.size() > 0)
		{
			tracks.clear();
			tracks.reserve(detections.size());
			for_each(begin(detections), end(detections), [&](detection& d){
				initializeTrack(d, idGenerator, currentGrayFrame);
			});
			continue;
		}
#pragma endregion

#pragma region predict track positions for time t
		vector<track> tracksToErase;
		auto lkoutput = currentFrame.clone();

		auto it = tracks.begin();
		auto tend = tracks.end();
		for(; it != tend; ++it)
		{				
			if(trackHasExited(*it, currentFrame))				
			{
				tracksToErase.push_back(*it);
				continue;
			}

			//Rect lkRect, kalmanRect;
			//bool lkSuccess = trackLucasKanade(*it, frameBuffer, grayFrameBuffer, foregroundBuffer,lkRect,lkoutput);				
			//bool kalmanSuccess = predictKalman(*it, kalmanRect);
			//mergePredictions(lkSuccess, kalmanSuccess ,*it, lkRect, kalmanRect, grayFrameBuffer);
		}

		for_each(begin(tracksToErase), end(tracksToErase), [&](track tr){
			deleteTrack(tr);
		});	

		beginTracking();
		for(auto it = begin(tracks); it != end(tracks); it++)		
			registerForLucasKanade(*it);			
				
		performTracking();

		for(auto it = begin(tracks); it != end(tracks); it++)
		{
			Rect predictedRect, kalmanRect;
			bool lkSuccess = unregister(*it, predictedRect);
			bool kalmanSuccess = predictKalman(*it, kalmanRect);
			mergePredictions(lkSuccess, kalmanSuccess, *it, predictedRect, kalmanRect, grayFrameBuffer);
		}
			

#pragma endregion

		std::map<int, trackMatch> trMatches; 
		std::map<int, detectionMatch> detMatches;	

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

				float dist = secMatcher->match(tr, *dit, currentGrayFrame);
				if(dist > secMatcher->goodMaxDist)
				{
					//cv::waitKey();
					continue;
				}

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

#pragma region update track models and kalman
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

#pragma region init new tracks from unmatched detections
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
		Helper::drawDetections(detections, fclone);			

		Helper::drawTracks(tracks, fclone, Scalar(255,0,0));

		for_each(begin(tracks), end(tracks), [&](track& tr){
			Helper::drawRect(tr.model.kalmanRect, fclone, Scalar(0,0,255));
		});

		//imshow("xxx", currentForeground);
		//cv::waitKey(50);

		std::stringstream str;
		str << carCount;			
		Helper::drawText(str.str(), Point(10,20), fclone, Scalar(255,255,0));

		imshow("kalman", fclone);
		
		fclone.release();
		lkoutput.release();
#pragma endregion
#endif

		cv::waitKey(1.);
		send(syncBuffer,1);			
	}
}

void PTracker::beginTracking()
{
	allRegisteredPoints.clear();
	registeredStatuses.clear();
	registeredTracks.clear();
	xmedianForTracks.clear();
	ymedianForTracks.clear();
	scaleForTracks.clear();
	trackKey = 0;
}

int PTracker::registerForLucasKanade( track& tr )
{
	int id = trackKey++;
	auto trackRect = tr.asRecti();
	Mat currentForeground = foregroundBuffer[0];

	if(trackRect.area() < 10 || !Tool::rectInside(trackRect, frameRect))
	{
		registeredStatuses[id] = false;
		tr.lkId = id;
		return id;
	}
		
	Mat trackForeground = currentForeground(trackRect);

	int xoffset = trackRect.width / XCount;
	int yoffset = trackRect.height / YCount;
	Point tl = trackRect.tl();

	vector<Point2f> trackedPoints; 
	for(int i = 0; i < XCount; i++)
	{
		for(int j = 0; j < YCount; j++)
		{
			int line = yoffset*j;
			int col = xoffset*i;						
			uchar value = trackForeground.at<uchar>(line,col);
			if( value == 255)
				trackedPoints.push_back(Point2f(tl.x, tl.y) + Point2f(col,line));			
		}
	}

	if(trackedPoints.size() < minTrackedPoints)
	{
		registeredStatuses[id] = false;
		tr.lkId = id;
		return id;
	}
		
	int begin = allRegisteredPoints.size();
	allRegisteredPoints.insert(allRegisteredPoints.end(), trackedPoints.begin(), trackedPoints.end());
	int end = allRegisteredPoints.size() - 1;
	trackEntry entry = {begin, end, id};
	registeredTracks[id] = entry;	
	tr.lkId = id;
	return id;
}

void PTracker::performTracking()
{	
	if(allRegisteredPoints.size() == 0)
		return;

	vector<Point2f> ftracked;
	vector<Point2f> btracked;
	vector<uchar> fstatus, bstatus;
	vector<float> ferrors, berrors;

	cv::calcOpticalFlowPyrLK(
		grayFrameBuffer[0],			
		grayFrameBuffer[1], 
		allRegisteredPoints, 
		ftracked,
		fstatus,
		ferrors	
	);

	cv::calcOpticalFlowPyrLK(
		grayFrameBuffer[1],			
		grayFrameBuffer[0],
		ftracked,
		btracked,
		bstatus,
		berrors
	);


	for(auto it = begin(registeredTracks); it != end(registeredTracks); ++it)
	{
		int begin = (*it).second.begin;
		int end = (*it).second.end;
		int id = (*it).second.id;				

		int sz = end - begin + 1;
		float* xdiffs = new float[sz];
		float* ydiffs = new float[sz];
		int successCount = 0;
		
		for(int i = begin; i <= end; i++)
		{
			if(fstatus[i] == 1 && bstatus[i] == 1)
			{
				auto vec = btracked[i] - allRegisteredPoints[i];
				float dist = sqrt(vec.x * vec.x + vec.y * vec.y);		
				if(dist < treshold)
				{					
					Point2f diff = ftracked[i] - allRegisteredPoints[i];					
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
	
		if(successCount > minTrackedPoints)
		{
			float xmedian = Tool::median(xdiffs, successCount);
			float ymedian = Tool::median(ydiffs, successCount);
		
			float *scales = new float[sz*(sz-1)/2];
			int comparisons = 0;
			for (int i = begin; i < end; i++) {
				for (int j = i + 1; j <= end; j++) {
					if (fstatus[i] == 1 && fstatus[j] == 1) {

						float dxPrev = allRegisteredPoints[j].x - allRegisteredPoints[i].x;
						float dyPrev = allRegisteredPoints[j].y - allRegisteredPoints[i].y;

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

			xmedianForTracks[id] = xmedian;
			ymedianForTracks[id] = ymedian;
			scaleForTracks[id] = scale;
			registeredStatuses[id] = true;
		}else
		{
			registeredStatuses[id] = false;
		}	

		delete[] xdiffs;
		delete[] ydiffs;	
	}
}

bool PTracker::unregister(track& tr, Rect& predictedRect )
{
	if(registeredStatuses.find(tr.lkId) == registeredStatuses.end())
		assert(false, "this should not happen");

	if(registeredStatuses[tr.lkId] == false)
		return false;
	
	predictedRect = tr.asRecti();
	Tool::moveRect(predictedRect, xmedianForTracks[tr.lkId], ymedianForTracks[tr.lkId]);
	float scale = scaleForTracks[tr.lkId];
	Tool::scaleRect(predictedRect, scale, scale);
	predictedRect = frameRect & predictedRect;

	return true;
}

