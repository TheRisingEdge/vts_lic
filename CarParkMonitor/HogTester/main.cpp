#include <stdio.h>
#include "Content.h"
#include "VideoProcessor.h"
#include "AvgSubtractor.h"
#include "Tool.h"
#include "MogSubtractor.h"
#include "HogTest.h"

using namespace cv;

int main(int argc, char** argv)
{
	char* path = new char[50];
	sprintf(path,"../CarParkMonitor/Content/Videos/%s",argv[1]);

	auto processor = new VideoProcessor(path);
	FrameProcessor* fproc = new HogTest();

	processor->run(fproc);

	delete[] path;
	return 0;
}