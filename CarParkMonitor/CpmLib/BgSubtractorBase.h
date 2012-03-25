#pragma once

#include <opencv2\opencv.hpp>

class BgSubtractorBase
{
public:
	BgSubtractorBase(void){};
	~BgSubtractorBase(void){};

	virtual void learn(const cv::Mat& frame) = 0;
	virtual cv::Mat segment(const cv::Mat& frame) = 0;
};

