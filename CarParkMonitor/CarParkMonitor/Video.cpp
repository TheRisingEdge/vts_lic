#include "Video.h"

bool Video::openCapture( int& fps, double& frameDelay )
{
	this->capture.open(this->videoPath);
	if (!capture.isOpened())
	{
		capture.release();
		return false;
	}

	fps        = capture.get(CV_CAP_PROP_FPS);
	frameDelay = 1000.0 / (double)(fps);
	return true;
}

bool Video::read( Mat& frame )
{
	frameCount++;
	return capture.read(frame);
}

