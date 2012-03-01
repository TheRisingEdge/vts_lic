#include "AutoTracker.h"
#include "Blob.h"

AutoTracker::AutoTracker( AutoTrackerParam param )
{
	this->videoPath = param.videoPath;
	this->foregroungSegmentator = param.foregroundSegmentator;
	this->blobDetector = param.blobDetector;
	this->blobTracker = param.blobTracker;
	this->trackHistory = param.trackHistory;

	assert(this->videoPath);
	assert(this->foregroungSegmentator);
	assert(this->blobDetector);
	assert(this->blobTracker);
	assert(this->trackHistory);
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

	DetectorParams detectorParam;
	MatcherParams matcherParam;
	MatcherResult matcherResult;

	vector<blob> detectedBlobs;

	Mat frame;
	Mat prevFrame;
	Mat foreground;
	Mat prevForeground;

	capture.read(prevFrame);
	while(capture.read(frame)){
			
		Mat foregroundMask = foregroungSegmentator->segment(frame);
		foreground = foregroundMask;

		detectorParam.frame = frame;
		detectorParam.prevFrame = prevFrame;
		detectorParam.foreground = foregroundMask;
		blobDetector->detect(detectorParam,&detectedBlobs);
		
		matcherParam.frame = frame;
		matcherParam.previousFrame = prevFrame;
		matcherParam.foreground = foregroundMask;
		matcherParam.previousForeground = prevForeground;
		matcherParam.detectedBlobs = &detectedBlobs;
		blobTracker->match(matcherParam, &matcherResult);

		trackHistory->previousBlobs = detectedBlobs;
		//trackHistory->update();

		imshow("frame", frame);
		imshow("foreground", foregroundMask);

		cv::swap(prevFrame, frame);	
		cv::swap(prevForeground, foreground);
		waitKey(frameDelay/5);
	}	
}
