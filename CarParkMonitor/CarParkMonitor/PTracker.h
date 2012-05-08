#pragma once
#include "Video.h"
#include "BgSubtractorBase.h"
#include "ClassifierBase.h"
#include "TrackMatcher.h"
#include "KalmanFilter2D.h"
#include "LbpMatcher.h"
#include "Validator.h"
#include "NccMatcher.h"

class PTracker
{
private:
	long long carCount;

	Rect frameRect;
	int frameCount;
	Video& video;
	BgSubtractorBase* subtractor;
	ClassifierBase* classifier;
	TrackMatcher* matcher;
	LbpMatcher* secMatcher;

	vector<track> tracks;	
	std::map<int, shared_ptr<KalmanFilter2D> > kalmanFilters;
	std::map<int, Validator> validators;

	track createFromDetection(detection& d,IdGenerator& gen, Mat& frame);
	track initializeTrack(detection& det, IdGenerator& idGenerator, Mat& grayFrame);
	void deleteTrack(track& tr);

#pragma region frame_buffers
	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<detection>> detectionBuffer;

	bool shiftBuffers(Mat frame);
#pragma endregion
	
	bool trackLucasKanade(track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds,Rect& predictedRect, Mat& output);		
	bool predictKalman(track& tr, Rect& predictedRect);
	void forwardKalman(track& tr);
	void mergePredictions(bool lkSuccess, bool kalmanSuccess, track& tr,Rect& lkRect,Rect& kalmanRect,vector<Mat> frameBuffer);

	void correctKalman(track& tr);
	bool trackHasExited(track& tr, Mat frame);

public:
	~PTracker(){};
	PTracker(Video& vid, BgSubtractorBase* subtr, ClassifierBase* cls, TrackMatcher* mat):
		video(vid),
		subtractor(subtr),
		classifier(cls),
		matcher(mat),
		carCount(0)
	{
		secMatcher = new LbpMatcher();
		//secMatcher = new NccMatcher();
	};
	
	void start();
};