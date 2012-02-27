#include "opencv2\opencv.hpp"
#include "ForegroundSegmentator.h"
#include "BlobDetector.h"

using namespace cv;


typedef struct
{
	char* videoPath;
	int trainingFrames;

	ForegroundSegmentator* foregroundSegmentator;
	BlobDetector* blobDetector;

}AutoTrackerParam;

class AutoTracker
{

private:


public:
	AutoTracker(AutoTrackerParam param);
    ~AutoTracker();

	

};