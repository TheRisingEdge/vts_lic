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

void AutoTracker::run()
{
	int fps;
	double frameDelay;
	int frameCount;

	if(!openVideoCapture(&fps, &frameDelay))	
		return;

	namedWindow("frame");

	IdGenerator* idGenerator = new IdGenerator(1);

	DetectorParams detectorParam;
	TrackerParam matcherParam;
	TrackParam trackParam;
	MatcherResult matcherResult;


	Mat frame, grayFrame;
	Mat prevFrame, prevGrayFrame;
	Mat foreground;
	Mat prevForeground;

	int trainingFrames = 50;


	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<blob*>> blobBuffer;

	int frameBufferSize = 10;

	frameCount = 1;

	capture.read(prevFrame);	
	while(capture.read(frame)){

		frameCount++;		
		cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

		frameBuffer.insert(frameBuffer.begin(), frame.clone());			
		grayFrameBuffer.insert(grayFrameBuffer.begin(), grayFrame.clone());

		frameBuffer.resize(frameBufferSize);
		grayFrameBuffer.resize(frameBufferSize);

		//**Foreground segmentation**************************************//
		Mat foregroundMask = foregroungSegmentator->segment(frame); // use background model to segment frame
		foregroundBuffer.insert(foregroundBuffer.begin(), foregroundMask.clone());
		foregroundBuffer.resize(frameBufferSize);


		//**Detect blobs************************************************//
		detectorParam.frame 	 = frame;
		detectorParam.prevFrame  = prevFrame;
		detectorParam.foreground = foregroundMask;
		vector<blob*> foundBlobs = blobDetector->detect1(detectorParam);
		blobBuffer.insert(blobBuffer.begin(), foundBlobs);
		blobBuffer.resize(frameBufferSize);


		if(frameCount < trainingFrames)
		{//construct background model



		}else
		{//detection and classification

			foreground = foregroundMask;		
			
			//** setup Tracker parameters *******************************//
			trackerParam.frame 				= frameBuffer[frameBufferSize-1];
			trackerParam.grayFrame 			= grayFrameBuffer[frameBufferSize-1];
			trackerParam.previousFrame 		= prevFrame;
			trackerParam.previousGrayFrame 	= prevGrayFrame;		

			trackerParam.foreground 		= foregroundBuffer[frameBufferSize-1];
			trackerParam.previousForeground = prevForeground;

			trackerParam.detectedBlobs 		= blobBuffer[frameBufferSize-1];
			trackerParam.generator 			= idGenerator;		
			trackerParam.frameCount			= frameCount;

			trackerParam.frameBufferSize    = frameBufferSize-1;
			trackerParam.frameBuffer 		= frameBuffer;
			trackerParam.grayFrameBuffer    = grayFrameBuffer;
			trackerParam.foregroundBuffer   = foregroundBuffer;
			
			matcherResult.init();

			blobTracker->track(trackerParam, &matcherResult);

			
			//** Assign ids for new detected Blobs *********************//
			vector<blob*>::iterator it = matcherResult.newBlobs.begin();
			vector<blob*>::iterator end = matcherResult.newBlobs.end();
			for(;it != end; it++)
			{
				blob* b = *it;
				assert(b->id == ID_UNDEFINED);
				b->id = idGenerator->nextId();			
			}

			//delete previous matched blobs




			//** Update Tracking history ******************************//
			trackHistory->previousBlobs = blobBuffer[frameBufferSize-1];
			//trackParam.trackedBlobs = detectedBlobs;		
			//trackParam.prevLostBlobs = matcherResult.prevLostBlobs;

			//trackHistory->update(&trackParam);
		}
	
		if(frameCount > frameBufferSize)
		{
		
			
			imshow("frame", frameBuffer[frameBufferSize-1]);

			prevFrame 	   = frameBuffer[frameBufferSize-1];
			prevGrayFrame  = grayFrameBuffer[frameBufferSize-1];
			prevForeground = foregroundBuffer[frameBufferSize-1];
		}

		if(waitKey(frameDelay/5) >= 0)
		{
			frameDelay = 1000000;
		}				
	}	
}
