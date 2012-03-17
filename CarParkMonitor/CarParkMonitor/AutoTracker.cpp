#include "AutoTracker.h"
#include "Blob.h"
#include "Helper.h"
#include <algorithm>

using namespace std;

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

#if FRAME_DRAW
	namedWindow("frame");
#endif
	
	IdGenerator* idGenerator = new IdGenerator(1);

	DetectorParams detectorParam;
	TrackerParam matcherParam;
	TrackParam trackParam;
	MatcherResult matcherResult;

	Mat frame;
	Mat grayFrame;
	Mat foreground;	

	int trainingFrames = 50;

	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<blob*>> blobBuffer;
	
	frameCount = 0;
	int frameBufferSize = 10;

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
		detectorParam.foreground = foregroundMask;
		vector<blob*> foundBlobs = blobDetector->detect(detectorParam);
		blobBuffer.push_back(foundBlobs);


		if(frameBuffer.size() > frameBufferSize)
		{			
			frameBuffer[0].release();
			grayFrameBuffer[0].release();
			foregroundBuffer[0].release();

			frameBuffer.erase(frameBuffer.begin());
			grayFrameBuffer.erase(grayFrameBuffer.begin());
			foregroundBuffer.erase(foregroundBuffer.begin());
			blobBuffer.erase(blobBuffer.begin());
		}

		if(frameCount < trainingFrames)
		{//construct background model

			foregroungSegmentator->learn(frame);

		}else
		{//detection and classification

			foreground = foregroundMask;		

			auto prev = blobBuffer[0];
			for_each(begin(prev), end(prev), [&](blob* b)-> void{
				if(b->id == ID_UNDEFINED)
					b->id = idGenerator->nextId();
			});

			//auto c = blobBuffer[1];
			//for_each(begin(c), end(c), [&](blob* b){
			//	if(b->id != ID_UNDEFINED)
			//	{
			//		int a;
			//		a = 34;
			//	}
			//});

			//** setup Tracker parameters *******************************//			
			trackerParam.generator 			= idGenerator;		
			trackerParam.frameCount			= frameCount;
			
			trackerParam.frameBufferSize    = frameBufferSize;
			trackerParam.frameBuffer 		= frameBuffer;
			trackerParam.grayFrameBuffer    = grayFrameBuffer;
			trackerParam.foregroundBuffer   = foregroundBuffer;
			trackerParam.blobBuffer			= blobBuffer;
			
			matcherResult.init();
			blobTracker->track(trackerParam, &matcherResult);
			
			//** Assign ids for new detected Blobs *********************//
			auto blobsNew = matcherResult.newBlobs;
			for_each(begin(blobsNew), end(blobsNew), [&](blob* b)
			{
				assert(b->id == ID_UNDEFINED);
				//b->id = idGenerator->nextId();
			});

			//drawCurrentBlobs
			auto fclone = frameBuffer[1].clone();
			auto currentBlobs = blobBuffer[1];
			for_each(begin(currentBlobs), end(currentBlobs), [&](blob* b)
			{
				Helper::drawBlob(b, fclone);									
			});
			imshow("labeled Blobs", fclone);
			fclone.release();

		}
#if FRAME_DRAW

		if(frameCount > frameBufferSize)
		{			
			imshow("frame", frameBuffer[1]);
		}

		/*if(waitKey(frameDelay/5) >= 0)
		{
			frameDelay = 1000000;
		}*/				
#endif
		
		waitKey(frameDelay);		
	}	
}
