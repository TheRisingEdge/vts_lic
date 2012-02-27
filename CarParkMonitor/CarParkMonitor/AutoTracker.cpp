#include "AutoTracker.h"
#include "Blob.h"

AutoTracker::AutoTracker( AutoTrackerParam param )
{
	this->videoPath = param.videoPath;
	this->foregroungSegmentator = param.foregroundSegmentator;
	this->blobDetector = param.blobDetector;

	assert(this->videoPath);
	assert(this->foregroungSegmentator);
	assert(this->blobDetector);
}

AutoTracker::~AutoTracker()
{

}

bool AutoTracker::openVideoCapture(int* fps, double* frameDelay)
{
	this->capture.open(this->videoPath);

	if(!capture.isOpened())
	{
		capture.release();
		return false;
	}
	
	*fps = capture.get(CV_CAP_PROP_FPS);		
	*frameDelay = 1000.0/(double)(*fps);

	return true;
}

void AutoTracker::process()
{
	int fps;
	double frameDelay;

	if(!openVideoCapture(&fps, &frameDelay))	
		return;

	namedWindow("foreground");
	namedWindow("frame");

	BlobSeq trackedBlobs(NULL);
	BlobSeq newBlobs(NULL);

	Mat frame;
	while(capture.read(frame)){
	
		imshow("frame", frame);

		Mat foregroundMask = foregroungSegmentator->process(frame);
		imshow("foreground", foregroundMask);

		blobDetector->process(frame, foregroundMask, trackedBlobs, &newBlobs);

		waitKey(frameDelay);
	}	
}
