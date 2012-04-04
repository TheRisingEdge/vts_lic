#pragma once
#include "BgSubtractorBase.h"
#include <opencv2\opencv.hpp>

using namespace cv;

class StableMog: public BgSubtractorBase
{
private:
	char* meanWindowName;
	char* foregroundWindowName;

	BackgroundSubtractorMOG2 bgModel;
	Mat foreground;
	Mat maskedForeground;

	Mat rectKernel;
	Mat elipseKernel;
	Mat crossKernel;
	Mat smallCrossKernel;

	void init(char* windowName);

public:
	StableMog(char* windowName);
	StableMog();
	virtual ~StableMog(){}

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
	Mat getBackground();
};

