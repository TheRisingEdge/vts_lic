#include "BgFgSegmentator.h"

BgFgSegmentator::BgFgSegmentator() :FrameProcessor("BgFg"), threshold(10), learningRate(0.01)
{

}

void BgFgSegmentator::setThreshold( int t )
{
	this->threshold = t;
}

void BgFgSegmentator::setLearningRate( double r )
{
	this->learningRate = r;
}

void BgFgSegmentator::process( cv:: Mat frame, cv:: Mat* output )
{
	// convert to gray-level image
	cv::cvtColor(frame, gray, CV_BGR2GRAY); 

	// initialize background to 1st frame
	if (background.empty())
		gray.convertTo(background, CV_32F);

	// convert background to 8U
	background.convertTo(backImage,CV_8U);

	// compute difference between current image and background
	cv::absdiff(backImage,gray,foreground);

	// apply threshold to foreground image
	cv::threshold(foreground,*output,threshold,255,cv::THRESH_BINARY_INV);

	// accumulate background
	cv::accumulateWeighted(gray, background, learningRate, *output);
}


