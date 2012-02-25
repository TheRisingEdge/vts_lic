#include "CannyFrameProcessor.h"

CannyFrameProcessor::CannyFrameProcessor():FrameProcessor("Canny Detection")
{

}

CannyFrameProcessor::~CannyFrameProcessor(void)
{
}

void CannyFrameProcessor::process( cv::Mat in, cv::Mat* out )
{
	in.copyTo(*out);
	if (in.channels() == 3)
		cv::cvtColor(in,*out,CV_BGR2GRAY);
	
	// Compute Canny edges
	cv::Canny(*out,*out,100,200);

	// Invert the image
	cv::threshold(*out,*out,128,255,cv::THRESH_BINARY_INV);

}

