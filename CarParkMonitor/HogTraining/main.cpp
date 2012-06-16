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

	char* config = new char[100];
	sprintf(config,"../CarParkMonitor/Content/Config/%s",argv[2]);
	auto cf = Content::read(config);

	int x, y, width, height = 0;
	cf["x"] >> x;
	cf["y"] >> y;
	cf["width"] >> width;
	cf["height"] >> height;
	cf.release();

	auto processor = new VideoProcessor(path);

	HogFramerParams params;
	params.cropFrame = Rect(0, 0, width, height);
	RectTool::toCenter(Point(x,y), params.cropFrame);

	auto hogTrainer = new HogFramer(params);
	processor->run(hogTrainer, 10.0);

	delete[] path;
	return 0;
}