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

bool AutoTracker::openVideoCapture(int& fps, double& frameDelay)
{
	this->capture.open(this->videoPath);

	if(!capture.isOpened())
	{
		capture.release();
		return false;
	}
	
	fps = capture.get(CV_CAP_PROP_FPS);		
	frameDelay = 1000.0/(double)(fps);

	return true;
}

void AutoTracker::run()
{
	int fps;
	double frameDelay;
	int frameCount;

	if(!openVideoCapture(fps, frameDelay))	
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

		frameBuffer.push_back(frame.clone());
		grayFrameBuffer.push_back(grayFrame.clone());

		//**Foreground segmentation**************************************//
		Mat foregroundMask = foregroungSegmentator->segment(frame); // use background model to segment frame
		foregroundBuffer.push_back(foregroundMask.clone());
		
		//**Detect blobs************************************************//
		detectorParam.frame 	 = frame;
		detectorParam.prevFrame  = prevFrame;
		detectorParam.foreground = foregroundMask;
		vector<blob*> foundBlobs = blobDetector->detect(detectorParam);
		blobBuffer.push_back(foundBlobs);


		if(frameBuffer.size() > frameBufferSize)
		{
			frameBuffer.erase(frameBuffer.begin());
			grayFrameBuffer.erase(grayFrameBuffer.begin());
			foregroundBuffer.erase(foregroundBuffer.begin());
			blobBuffer.erase(blobBuffer.begin());
		}

		if(frameCount < trainingFrames)
		{//construct background model



		}else
		{//detection and classification

			foreground = foregroundMask;		

			//** setup Tracker parameters *******************************//			
			trackerParam.generator 			= idGenerator;		
			trackerParam.frameCount			= frameCount;
			
			trackerParam.prevFrame 			= prevFrame;
			trackerParam.prevGrayFrame		= prevGrayFrame;		
			trackerParam.prevForeground		= prevForeground;
			
			trackerParam.frameBufferSize    = frameBufferSize;
			trackerParam.frameBuffer 		= frameBuffer;
			trackerParam.grayFrameBuffer    = grayFrameBuffer;
			trackerParam.foregroundBuffer   = foregroundBuffer;
			trackerParam.blobBuffer			= blobBuffer;
			
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
			trackHistory->previousBlobs = blobBuffer[0];
			//trackParam.trackedBlobs = detectedBlobs;		
			//trackParam.prevLostBlobs = matcherResult.prevLostBlobs;

			//trackHistory->update(&trackParam);
		}
	
		if(frameCount > frameBufferSize)
		{
		
			
			imshow("frame", frameBuffer[0]);

			prevFrame 	   = frameBuffer[0];
			prevGrayFrame  = grayFrameBuffer[0];
			prevForeground = foregroundBuffer[0];
		}

		if(waitKey(frameDelay/5) >= 0)
		{
			frameDelay = 1000000;
		}				
	}	
}
