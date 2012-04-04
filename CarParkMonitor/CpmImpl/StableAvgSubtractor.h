#pragma once
#include <opencv2\opencv.hpp>
#include "BgSubtractorBase.h"

using namespace cv;

class StableAvgSubtractor: public BgSubtractorBase
{
private:
	cv::Mat grayFrame;			// current gray-level image
	cv::Mat background;		// accumulated background
	cv::Mat backgroundGray;		// background image
	cv::Mat foreground;		// foreground image
	cv::Mat newForeground;

	double learningRate;    // learning rate in background accumulation
	int threshold;			// threshold for foreground extraction

	int closeHolesCount;
	Mat structuringElement;
	Mat elipseElement;

	void init(char* windowName);

public:
	StableAvgSubtractor();
	StableAvgSubtractor(char* windowName);
	~StableAvgSubtractor();

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
};

