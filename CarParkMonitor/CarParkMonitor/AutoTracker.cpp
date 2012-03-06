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
	int frameCount;

	if(!openVideoCapture(&fps, &frameDelay))	
		return;

	namedWindow("frame");

	IdGenerator* idGenerator = new IdGenerator(1);

	DetectorParams detectorParam;
	MatcherParams matcherParam;
	TrackParam trackParam;
	MatcherResult matcherResult;

	vector<blob*> detectedBlobs;

	Mat frame, grayFrame;
	Mat prevFrame, prevGrayFrame;
	Mat foreground;
	Mat prevForeground;

	frameCount = 1;
	capture.read(prevFrame);
	while(capture.read(frame)){

		frameCount++;		

		cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);		

		Mat foregroundMask = foregroungSegmentator->segment(frame);
		foreground = foregroundMask;		

		detectorParam.frame = frame;
		detectorParam.prevFrame = prevFrame;
		detectorParam.foreground = foregroundMask;
		blobDetector->detect(detectorParam,&detectedBlobs);
		
		matcherParam.frame 				= frame;
		matcherParam.grayFrame 			= grayFrame;
		matcherParam.previousFrame 		= prevFrame;
		matcherParam.previousGrayFrame 		= prevGrayFrame;		

		matcherParam.foreground 		= foregroundMask;
		matcherParam.previousForeground = prevForeground;

		matcherParam.detectedBlobs 		= detectedBlobs;
		matcherParam.generator 			= idGenerator;		
		matcherParam.frameCount			= frameCount;
		
		matcherResult.init();
		blobTracker->match(matcherParam, &matcherResult);
		
		vector<blob*>::iterator it = matcherResult.newBlobs.begin();
		vector<blob*>::iterator end = matcherResult.newBlobs.end();
		for(;it != end; it++)
		{
			blob* b = *it;
			assert(b->id == ID_UNDEFINED);
			b->id = idGenerator->nextId();			
		}

		trackHistory->previousBlobs = detectedBlobs;
		trackParam.trackedBlobs = detectedBlobs;		
		trackParam.prevLostBlobs = matcherResult.prevLostBlobs;

		trackHistory->update(&trackParam);

		imshow("frame", frame);

		cv::swap(prevFrame, frame);
		cv::swap(prevGrayFrame, grayFrame);
		cv::swap(prevForeground, foreground);
		if(waitKey(frameDelay/5) >= 0)
		{
			frameDelay = 1000000;
		}		
	}	
}
