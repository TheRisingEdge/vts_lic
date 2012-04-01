#pragma once
#include "FrameProcessor.h"
#include "opencv2\opencv.hpp"
#include <opencv2\gpu\gpu.hpp>

using namespace cv;
using namespace cv::gpu;

class HogTest: public FrameProcessor
{
private:
	cv::HOGDescriptor* hog;
	cv::gpu::HOGDescriptor* hog_gpu;

public:
	HogTest(void);
	~HogTest(void){};
	void process(const Mat& frame);

};

