#pragma once
#include "Video.h"
#include "BgSubtractorBase.h"
#include "ClassifierBase.h"
#include "TrackMatcher.h"
#include "KalmanFilter2D.h"
#include "LbpMatcher.h"
#include "Validator.h"
#include "NccMatcher.h"
#include "PClassifier.h"
#include "PSubtractor.h"
#include <agents.h>

using namespace Concurrency;

class PTracker: public agent
{
private:
#pragma region fields
	long long carCount;
	Rect frameRect;
	int frameCount;
		
	TrackMatcher* matcher;
	TrackMatcher* secMatcher;

	vector<track> tracks;	
	std::map<int, shared_ptr<KalmanFilter2D> > kalmanFilters;
	std::map<int, Validator> validators;
#pragma endregion
	
#pragma region frame_buffers
	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<detection>> detectionBuffer;

	bool shiftBuffers(SubFrame subResult, ClasifierFrame classifierResult);
#pragma endregion
	
#pragma region methods
	track createFromDetection(detection& d,IdGenerator& gen, Mat& frame);
	track initializeTrack(detection& det, IdGenerator& idGenerator, Mat& grayFrame);
	void deleteTrack(track& tr);

	bool trackLucasKanade(track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds,Rect& predictedRect, Mat& output);		
	bool predictKalman(track& tr, Rect& predictedRect);
	void forwardKalman(track& tr);
	void mergePredictions(bool lkSuccess, bool kalmanSuccess, track& tr,Rect& lkRect,Rect& kalmanRect,vector<Mat> frameBuffer);

	void correctKalman(track& tr);
	bool trackHasExited(track& tr, Mat frame);
#pragma endregion

	ISource<ClasifierFrame>& classifierBuffer;
	ISource<SubFrame>& subtractorBuffer;
	ITarget<int>& syncBuffer;

public:
	~PTracker(){}
	PTracker(ISource<SubFrame>& subBuffer, ISource<ClasifierFrame>& clsBuffer, TrackMatcher* mat, ITarget<int>& sync)
		:subtractorBuffer(subBuffer),
		classifierBuffer(clsBuffer),
		matcher(mat),
		syncBuffer(sync),
		carCount(0),
		secMatcher(new LbpMatcher())
	{}	

	void run();
};