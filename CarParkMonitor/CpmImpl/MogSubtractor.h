#pragma once
#include "BgSubtractorBase.h"
#include "opencv2/opencv.hpp"

using namespace cv;

class MogSubtractor : public BgSubtractorBase
{
private:
	char* meanWindowName;
	char* foregroundWindowName;

	BackgroundSubtractorMOG2 bgModel;
	Mat img;
	Mat foreground;
	Mat maskedForeground;
	Mat rectKernel;
	Mat elipseKernel;
	Mat crossKernel;
	Mat smallCrossKernel;

	void init(char* windowName);

public:
	MogSubtractor(char* windowName);
	MogSubtractor();
	virtual ~MogSubtractor(){}

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
};

