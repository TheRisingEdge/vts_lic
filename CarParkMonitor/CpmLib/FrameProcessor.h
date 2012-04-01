#pragma once
#include "opencv2/opencv.hpp"

class FrameProcessor
{	
	public:
		FrameProcessor(char* windowName);
		virtual void process(const cv::Mat& in) = 0;
};