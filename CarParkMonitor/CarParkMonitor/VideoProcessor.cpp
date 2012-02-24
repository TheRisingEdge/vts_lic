#include "VideoProcessor.h"


VideoProcessor::VideoProcessor(char* videoPath)
{
	this->videoPath = videoPath;
	this->windowName = "Video Processor";
}

VideoProcessor::~VideoProcessor(){}


void VideoProcessor::run(FrameProcessor* frameProcessor )
{	
	bool stop = false;
	this->capture.open(videoPath);

	if(!capture.isOpened()){
		stop = true;
	}

	double frameRate = capture.get(CV_CAP_PROP_FPS);		
	int delay = 1000/frameRate;

	cv::namedWindow(this->windowName);

	while(!stop){
		if(!capture.read(this->in)){
			break;
		}

		frameProcessor->process(this->in, &this->out);

		cv::imshow(windowName, this->out);
		if(cv::waitKey(delay)>=0){
			stop = true;
		}
	}

	capture.release();
}

void VideoProcessor::run( FrameProcessor* frameProcessors, int howMany )
{

}
