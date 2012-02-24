#pragma once
#include "opencv2/opencv.hpp"
#include "FrameProcessor.h"

class CannyFrameProcessor: public FrameProcessor
{
	public:
		CannyFrameProcessor(void);
		~CannyFrameProcessor(void);

		void process(cv::Mat in, cv::Mat* out);	
};

