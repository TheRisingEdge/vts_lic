#pragma once
#include "BgSubtractorBase.h"
#include "opencv2/opencv.hpp"

using namespace cv;

class Mog2Subtractor : public BgSubtractorBase
{
private:
	BackgroundSubtractorMOG bgModel;
	Mat img;
	Mat foreground;
	Mat maskedForeground;

public:
	Mog2Subtractor(void);
	~Mog2Subtractor(void);

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
};

