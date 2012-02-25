#pragma once
#include "FrameProcessor.h"


class BgFgSegmentator: public FrameProcessor
{
private:
	cv::Mat gray;			// current gray-level image
	cv::Mat background;		// accumulated background
	cv::Mat backImage;		// background image
	cv::Mat foreground;		// foreground image
	double learningRate;    // learning rate in background accumulation
	int threshold;			// threshold for foreground extraction

public:

	BgFgSegmentator();

	// Set the threshold used to declare a foreground
	void setThreshold(int t);

	// Set the learning rate
	void setLearningRate(double r);

	// processing method
	void process(cv:: Mat frame, cv:: Mat* output);
};

