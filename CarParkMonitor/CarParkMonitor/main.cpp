#include <stdio.h>
#include "AutoTracker.h"
#include "Content.h"


int main(int argc, char** argv)
{

	char* videoPath = Content::videoFile(argv[1]);

	AutoTrackerParam param;
	param.videoPath = videoPath;
	param.foregroundSegmentator = new ForegroundSegmentator();
	param.blobDetector = new BlobDetector();

	AutoTracker tracker(param);
	tracker.process();

	return 0;
}