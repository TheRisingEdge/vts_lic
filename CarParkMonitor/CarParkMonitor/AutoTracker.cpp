#include "AutoTracker.h"
#include "Blob.h"
#include "Helper.h"
#include <algorithm>
#include "EstimatorCollection.h"
#include "VehicleClassifier.h"

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
	TrackResult trackResult;

	Mat frame;
	Mat grayFrame;
	Mat foreground;	

	int trainingFrames = 10;

	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<shared_ptr<blob>>> blobBuffer;
	vector<vector<shared_ptr<carDetection>>> detectionBuffer;
	
	EstimatorCollection estimators;

	ClassifierParam classifierParam;
	ClassifierResult classifierResult;
	VehicleClassifier vehicleClassifier;

	frameCount = 0;
	int frameBufferSize = 10;

	while(capture.read(frame)){

		frameCount++;		

		if(frameCount < trainingFrames)
		{//construct background model

			foregroungSegmentator->learn(frame);

		}else
		{//detection and classification
					
			//**Foreground segmentation**************************************//
			Mat foregroundMask = foregroungSegmentator->segment(frame);
			
			cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

			auto frameClone = frame.clone();
			auto foregroundClone = foregroundMask.clone();

			frameBuffer.push_back(frameClone);
			grayFrameBuffer.push_back(grayFrame.clone());
			foregroundBuffer.push_back(foregroundClone);

			//**Detect blobs************************************************//
			detectorParam.frame 	 = frameClone;
			detectorParam.foreground = foregroundClone;
			vector<shared_ptr<blob>> foundBlobs = blobDetector->detect(detectorParam);
			blobBuffer.push_back(foundBlobs);

			//**setup Classifier parameters ******************************//
			classifierParam.blobBuffer = foundBlobs;
			classifierParam.frame 	   = frameClone;
			classifierParam.foreground = foregroundClone;
			vector<shared_ptr<carDetection>> detections = vehicleClassifier.detect(classifierParam);
			detectionBuffer.push_back(detections);

			if(frameBuffer.size() > frameBufferSize)
			{			
				frameBuffer[0].release();
				grayFrameBuffer[0].release();
				foregroundBuffer[0].release();
				detectionBuffer[0].clear();

				frameBuffer.erase(frameBuffer.begin());
				grayFrameBuffer.erase(grayFrameBuffer.begin());
				foregroundBuffer.erase(foregroundBuffer.begin());
				blobBuffer.erase(blobBuffer.begin());
				detectionBuffer.erase(detectionBuffer.begin());

				foreground = foregroundMask;		

				auto prev = detectionBuffer[0];
				for_each(begin(prev), end(prev), [&](shared_ptr<carDetection> b)-> void{
					if(b->id == ID_UNDEFINED)
						b->id = idGenerator->nextId();
				});
				
				//** setup Tracker parameters *******************************//			
				trackerParam.generator 			= idGenerator;		
				trackerParam.frameCount			= frameCount;

				trackerParam.frameBufferSize    = frameBufferSize;
				trackerParam.frameBuffer 		= frameBuffer;
				trackerParam.grayFrameBuffer    = grayFrameBuffer;
				trackerParam.foregroundBuffer   = foregroundBuffer;
				trackerParam.blobBuffer			= blobBuffer;
				trackerParam.vehicleDetectionBuffer = detectionBuffer;

				trackResult.init();
				blobTracker->track(trackerParam, &trackResult);

				//** Assign ids for new detected Blobs *********************//
				auto vehiclesNew = trackResult.newVehicleDetections;
				for_each(begin(vehiclesNew), end(vehiclesNew), [&](shared_ptr<carDetection> b)
				{
					assert(b->id == ID_UNDEFINED);
					b->id = idGenerator->nextId();
					//estimators.update(b);
				});

				//drawCurrentBlobs
				auto fclone = frameBuffer[1].clone();
				auto currentDetections = detectionBuffer[1];
				for_each(begin(currentDetections), end(currentDetections), [&](shared_ptr<carDetection> b)
				{
				//	estimators.update(b);
					//auto est = estimators.get(b->id);
					//auto lastPrediction = est->getLastPrediction();

					//Helper::drawBlob(b.get(), fclone);
					Helper::drawAnotatedRect(b->id, b->rect, fclone);
					//Helper::drawAnotatedRect(b->id,lastPrediction.toRect(), fclone);
				});

				//auto m = estimators.estimatorsMap;
				//auto it = m.begin();
				//auto stop = m.end();

				//for(;it != stop; it++ )
				//{
				//	auto filter = (*it).second;
				//	auto predict = filter->getLastPrediction();
				//	Helper::drawAnotatedRect(filter->vehicleId,predict.toRect(), fclone);
				//}

				imshow("labeled Blobs", fclone);
				fclone.release();
			}
		}

#if FRAME_DRAW

		if(frameCount > trainingFrames + 2)
		{			
			imshow("frame", frameBuffer[1]);
		}

		///if(waitKey(frameDelay/5) >= 0)
		//{
		//	frameDelay = 1000000;
		//}				
#endif
		
		waitKey(frameDelay);		
	}	
}
