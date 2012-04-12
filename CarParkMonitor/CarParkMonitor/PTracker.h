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
	void trackLK(track& tr, vector<Mat> frames, vector<Mat> foregrounds);

public:
	~PTracker(){};
	PTracker(Video& vid, BgSubtractorBase* subtr, ClassifierBase* cls):
		video(vid),
		subtractor(subtr),
		classifier(cls)	
	{};

	void start();
};

