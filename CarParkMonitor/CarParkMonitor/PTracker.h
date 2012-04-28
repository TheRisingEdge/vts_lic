#pragma once
#include "Video.h"
#include "BgSubtractorBase.h"
#include "ClassifierBase.h"
#include "TrackMatcher.h"
#include "KalmanFilter2D.h"

class PTracker
{
private:
	int frameCount;
	Video& video;
	BgSubtractorBase* subtractor;
	ClassifierBase* classifier;
	TrackMatcher* matcher;

	vector<track> tracks;	
	std::map<int, shared_ptr<KalmanFilter2D> > kalmanFilters;

	track initTrackFromDetection(const detection& d,IdGenerator& gen);
	track initializeTrack(detection& det, IdGenerator& idGenerator);
	void deleteTrack(track& tr);

	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<detection>> detectionBuffer;
	bool shiftBuffers(Mat frame);
	bool trackLucasKanade(track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds, Mat& output);		
	bool predictKalman(track& tr);
	void updateKalman(track& tr);
	bool trackHasExited(track& tr, Mat frame);

public:
	~PTracker(){};
	PTracker(Video& vid, BgSubtractorBase* subtr, ClassifierBase* cls, TrackMatcher* mat):
		video(vid),
		subtractor(subtr),
		classifier(cls),
		matcher(mat)
	{};
	
	void start();
};