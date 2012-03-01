#include "BlobTracker.h"

BlobTracker::BlobTracker( TrackHistory* trackHistory )
{
	this->trackHistory = trackHistory;
}


BlobTracker::~BlobTracker(void)
{
}

vector<Point2f> getGoodFeatures(Mat image)
{
	vector<Point2f> keypoints;

	cv::goodFeaturesToTrack(
		image, 		   // the image 
		keypoints,   	   // the output detected features
		20,  	   // the maximum number of features 
		0.001,     // quality level
		0.001);   	   // min distance between two features

	return keypoints;
}

vector<Point2f> getGoodFeatures(Mat image, Mat mask)
{
	vector<Point2f> keypoints;

	cv::goodFeaturesToTrack(
		image, 		   // the image 
		keypoints,   	   // the output detected features
		7,  	   // the maximum number of features 
		0.001,     // quality level
		0.001,
		mask
	);   	   // min distance between two features

	return keypoints;
}

void drawTrackedPoints(vector<Point2f> pts,cv:: Mat &output) {

	int size = pts.size();
	for(int i= 0; i < size; i++ ) {
		// draw line and circle		
		cv::circle(output, pts[i], 3, cv::Scalar(255,255,255),-1);
	}
}

void drawTrackedPoints(vector<Point2f> prev, vector<Point2f> curr, cv:: Mat &output) {
	int size = prev.size();
	for(int i= 0; i < size; i++ ) {
		// draw line and circle
		cv::line(output, prev[i], curr[i], cv::Scalar(255,255,255));
		cv::circle(output, curr[i], 2, cv::Scalar(255,255,255),-1);
	}
}


void BlobTracker::match( MatcherParams params, MatcherResult* result )
{
	vector<blob>* detectedBlobs = params.detectedBlobs;
	Mat frame = params.frame;
	Mat prevFrame = params.previousFrame;
	Mat prevForegroung = params.previousForeground;
	vector<blob> prevBlobs = trackHistory->previousBlobs;

	vector<Point2f> prevFrameKeypoints;
	vector<Point2f> currentFrameKeypoints;
	vector<uchar> trackingStatus;
	vector<float> trackingErrors;

	Mat grayFrame;
	Mat prevGrayFrame;

	cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);
	cv::cvtColor(prevFrame, prevGrayFrame, CV_BGR2GRAY);

	int size = prevBlobs.size();
	if(prevBlobs.size() == 0 && detectedBlobs->size() == 0)	
		return;
	
	
	for(int i = 0; i < size; i++)
	{
		blob* b = &(prevBlobs[i]);

		Rect blobRect = b->sourceRect;		 
		Mat blobImage = prevGrayFrame(blobRect);
		Mat blobForeground = prevForegroung(blobRect);

		vector<Point2f> keypoints = getGoodFeatures(blobImage, blobForeground);
		b->descriptor.pointFeatures = keypoints;
		Point2f tl = blobRect.tl();
		for(int j = 0; j < keypoints.size(); j++)
		{			
			prevFrameKeypoints.push_back(tl + keypoints[j]);
		}		
	}
	
	if(prevFrameKeypoints.size() == 0)
		return;

	cv::calcOpticalFlowPyrLK(
		prevGrayFrame,			
		grayFrame, 				// 2 consecutive images, must be gray
		prevFrameKeypoints, 	// input point position in first image
		currentFrameKeypoints, 	// output point postion in the second image
		trackingStatus,			// tracking success
		trackingErrors			// tracking error  	   
	);      			

	drawTrackedPoints(prevFrameKeypoints, currentFrameKeypoints, frame);	

	//for each detected blob filter keypoints with respect to its rect (it is a keypoint filter class)
	//map the keypoints back to a previous blob
	//find simple blob match

	//debug draw
}
