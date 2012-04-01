#include "VideoProcessor.h"


VideoProcessor::VideoProcessor(char* videoPath)
{
	this->videoPath = videoPath;
	this->windowName = "Video Processor";
}

VideoProcessor::~VideoProcessor(){}


void VideoProcessor::run(FrameProcessor* frameProcessor )
{	
	
	this->capture.open(this->videoPath);
	if(!capture.isOpened())
	{
		capture.release();
		return;
	}

	double fps = capture.get(CV_CAP_PROP_FPS);		
	
	while(capture.read(this->in)){

		//cv::imshow(this->windowName, this->in);
		
		frameProcessor->process(this->in);

		cv::waitKey(fps);		
	}

	capture.release();
}