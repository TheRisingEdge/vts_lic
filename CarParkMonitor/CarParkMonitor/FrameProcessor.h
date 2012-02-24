#pragma once
#include "opencv2/opencv.hpp"

class FrameProcessor
{	
	public:
		virtual void process(cv::Mat in, cv::Mat* out) = 0;
};

