#include "opencv2\opencv.hpp"
#include "BlobDetector.h"
#include "ForegroundSegmentator.h"

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
	char* videoPath;
	ForegroundSegmentator* foregroungSegmentator;
	BlobDetector* blobDetector;

	VideoCapture capture;	
	bool openVideoCapture(int* fps = NULL, double* frameDelay = NULL);

public:
	AutoTracker(AutoTrackerParam param);
    ~AutoTracker();

	void process();

};