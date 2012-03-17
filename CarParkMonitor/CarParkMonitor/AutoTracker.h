#include "opencv2\opencv.hpp"
#include "ForegroundSegmenter.h"
#include "BlobTracker.h"
#include "BlobDetector.h"

using namespace cv;

#define FRAME_DRAW 1

typedef struct
{
	char* videoPath;
	int trainingFrames;

	ForegroundSegmenter* foregroundSegmentator;
	BlobDetector* blobDetector;
	BlobTracker* blobTracker;
	TrackHistory* trackHistory;

}AutoTrackerParam;

class AutoTracker
{

private:
	char* videoPath;
	ForegroundSegmenter* foregroungSegmentator;
	BlobDetector* blobDetector;
	BlobTracker* blobTracker;
	TrackHistory* trackHistory;

	TrackerParam trackerParam;

	VideoCapture capture;	
	bool openVideoCapture(int& fps, double& frameDelay);

public:
	AutoTracker(AutoTrackerParam param);
    ~AutoTracker();

	void run();

};