#pragma once
#include "BgSubtractorBase.h"
#include "opencv2/opencv.hpp"

using namespace cv;

class MogSubtractor : public BgSubtractorBase
{
private:
	BackgroundSubtractorMOG2 bgModel;
	Mat img;
	Mat foreground;
	Mat maskedForeground;

public:
	MogSubtractor(void);
	~MogSubtractor(void);

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
};

