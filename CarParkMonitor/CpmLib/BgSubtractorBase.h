#pragma once

#include <opencv2\opencv.hpp>
using namespace cv;

class BgSubtractorBase
{
protected:
	char* windowName;
	virtual void init(char* windowName)
	{
		instanceNr++;
		this->windowName = windowName;
	}

public:
	static int instanceNr;

	BgSubtractorBase()
	{
		init("subtractor");
	}
	BgSubtractorBase(char* windowName)
	{
		init(windowName);
	}
	~BgSubtractorBase(){};

	virtual void learn(const cv::Mat& frame) = 0;
	virtual cv::Mat segment(const cv::Mat& frame) = 0;
	virtual Mat getBackground(){return Mat();}
};
