#pragma once
#include "BgSubtractorBase.h"
#include "ClassifierBase.h"
#include "Video.h"


struct model
{

};

struct track
{
	int id;
	Rect rect;
	model model;
};

class PTracker
{
private:
	Video& video;
	BgSubtractorBase* subtractor;
	ClassifierBase* classifier;	
	track initTrackFromDetection(const detection& d,IdGenerator& gen);
	vector<Point2f> secondFrameTrackedPoints;

	void trackLK(track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds);	
	vector<Point2f> forewardTrack(vector<Point2f> points, vector<Mat> frames);
	vector<Point2f> filterInliers(const vector<Point2f>& startPoints,const vector<Point2f>& backTrackedPoints, float treshold = 1.0);

	

public:
	~PTracker(){};
	PTracker(Video& vid, BgSubtractorBase* subtr, ClassifierBase* cls):
		video(vid),
		subtractor(subtr),
		classifier(cls)	
	{};

	void start();
};

