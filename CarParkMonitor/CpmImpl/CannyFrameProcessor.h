#pragma once
#include "opencv2/opencv.hpp"
#include "FrameProcessor.h"

class CannyFrameProcessor: public FrameProcessor
{
	public:
		CannyFrameProcessor();
		~CannyFrameProcessor(void);

		void process(const cv::Mat& in);	
};

