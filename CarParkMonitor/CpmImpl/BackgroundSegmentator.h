#pragma once
#include "FrameProcessor.h"

class BackgroundSegmentator: public FrameProcessor
{
	cv::BackgroundSubtractorMOG mog;

	public:
		~BackgroundSegmentator(void){};
		BackgroundSegmentator(void);;
			
		void process(const cv::Mat& in);
};

