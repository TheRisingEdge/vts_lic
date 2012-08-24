#include <stdio.h>
#include "Content.h"
#include "VideoProcessor.h"
#include "HogFramer.h"
#include "AvgSubtractor.h"
#include "RectTool.h"
#include "MogSubtractor.h"

using namespace cv;

int main(int argc, char** argv)
{
	char* path = new char[100];
	sprintf(path,"../CarParkMonitor/Content/Videos/%s",argv[1]);

	auto processor = new VideoProcessor(path);
	auto hogTrainer = new HogFramer();
	processor->run(hogTrainer, 10.0);

	delete[] path;
	return 0;
}