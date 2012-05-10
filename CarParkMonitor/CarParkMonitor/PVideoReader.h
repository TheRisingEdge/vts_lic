#pragma once
#include <agents.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace Concurrency;

class PVideoReader: public agent
{
private:
	char* _videoPath;
	ISource<int>& _syncBuffer;
	ITarget<Mat>& _outBuffer0;
	ITarget<Mat>& _outBuffer1;

public:
	PVideoReader(char* path, ITarget<Mat>& outBuffer0, ITarget<Mat>& outBuffer1 ,ISource<int>& syncBuffer)
		:_videoPath(path),
		_syncBuffer(syncBuffer),
		_outBuffer0(outBuffer0),
		_outBuffer1(outBuffer1)
	{}	

	virtual void run();
};

