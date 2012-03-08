#include "BlobTracker.h"
#include <assert.h>
#include "BlobDetector.h"
#include "AppConfig.h"



#define DIM_POINTS 5
#define TOTAL_POINTS (DIM_POINTS * DIM_POINTS)

BlobTracker::BlobTracker( TrackHistory* trackHistory )
{
	this->trackHistory = trackHistory;
	this->featureDetector = new cv::SiftFeatureDetector(0.001,0.005);	// new cv::GoodFeaturesToTrackDetector(
		//20, // maximum number of corners to be returned
		//0.01, // quality level
		//1); // minimum allowed distance between points;//
	this->descriptorExtractor = AppConfig::bowProperties.extractor;
}

float median(float *A, int length) {
	int index = (int)(length / 2);
	std::sort(A, A + length);

	if (length % 2 == 0) {
		return (A[index] + A[index + 1]) / 2;
	}
	else {
		return A[index];
	}
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
		10,  	   // the maximum number of features 
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
		//cv::line(output, prev[i], curr[i], cv::Scalar(255,255,255));
		cv::circle(output, curr[i], 2, cv::Scalar(255,255,255),-1);
	}
}

void drawBlobs(vector<blob*> blobs, Mat &output)
{
	int size = blobs.size();
	Scalar color = Scalar( 70, 255, 100 );
	Scalar red = Scalar( 0, 0, 255 );

	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int thickness = 1;  

	for( int i = 0; i< size; i++ )
	{
		Rect rect = blobs.at(i)->sourceRect;

		rectangle( output, rect.tl(), rect.br(), color);

		stringstream id;//create a stringstream
		id << blobs.at(i)->id;//add number to the stream

		string text = id.str();
				
		cv::Point textOrg = blobs.at(i)->sourceRect.tl();
		cv::putText(output, text, textOrg, fontFace, fontScale, red, thickness,8);
	}
}

//void rejectUntrackedFeatures(vector<Point2f>* points, vector<uchar> statuses){		
//	int k = 0;
//	int size = points->size();
//	for( int i = 0; size; i++ ) {
//		if (acceptTrackedPoint(i)) {			
//			initialFeaturePosition[k]= initialFeaturePosition[i];
//			points[1][k++] = points[1][i];
//		}
//	}
//
//	points[1].resize(k);
//	initialFeaturePosition.resize(k);
//}
//
//bool acceptTrackedPoint(int i) {
//	return featureStatuses[i] &&
//		// if point has moved
//		(abs(points[0][i].x-points[1][i].x)+
//		(abs(points[0][i].y-points[1][i].y))>2);
//}

int containedPointsInRect(Rect rect, vector<Point2f> pts)
{
	vector<Point2f>::iterator it = pts.begin();
	vector<Point2f>::iterator end = pts.end();
	
	int contained = 0;

	for(;it!=end; it++)
	{
		if(rect.contains(*it))
		{
			contained++;
		}
	}

	return contained;
}

void drawPoints(vector<Point2f> prev, cv:: Mat &output) {
	int size = prev.size();
	for(int i= 0; i < size; i++ ) {		
		cv::circle(output, prev[i], 2, cv::Scalar(255,255,255),-1);
	}
}

void BlobTracker::track( TrackerParam param, MatcherResult* result )
{
	this->trackerParam = param;

	int frameCount 				= param.frameCount;
	vector<blob*> detectedBlobs = param.detectedBlobs;
	Mat frame 					= param.frame;
	Mat foreground				= param.foreground;
	Mat prevFrame 				= param.previousFrame;
	Mat prevForeground 			= param.previousForeground;
	vector<blob*> prevBlobs 	= trackHistory->previousBlobs;

	vector<Point2f> prevFrameKeypoints;
	vector<Point2f> currentFrameKeypoints;
	vector<Point2f> correctTracked;
	vector<uchar> trackingStatus;
	vector<float> trackingErrors;

	Mat grayFrame 	  = param.grayFrame;
	Mat prevGrayFrame = param.previousGrayFrame;

	if(prevBlobs.size() == 0 && detectedBlobs.size() == 0)	
	{
		return;
	}

	if(prevBlobs.size() == 0 && detectedBlobs.size() != 0)
	{
		result->newBlobs.assign(detectedBlobs.begin(), detectedBlobs.end());
		return;
	}

	int size = prevBlobs.size();
	for(int i = 0; i < size; i++)
	{
		blob* b = prevBlobs[i];

		Rect blobRect = b->sourceRect;		 
		Mat blobImage = prevGrayFrame(blobRect);
		Mat blobForeground = prevForeground(blobRect);

		Point2f lower = blobRect.br();
		if(lower.x < 400 && lower.y < 400 )
		trackFB(blobRect, this->trackerParam.grayFrameBuffer);							
	}
}


void BlobTracker::trackFB( Rect r , vector<Mat> frames)
{
	// Perform Lucas-Kanade Tracking -----------------------------------------
	// Distribute points to track uniformly over the bounding-box
	double bbWidth = r.width;
	double bbHeight = r.height;
	double stepX = bbWidth / (DIM_POINTS + 1);
	double stepY = bbHeight / (DIM_POINTS + 1);
	int i, x, y;

	vector<Point2f> startPoints;
	vector<Point2f> endPoints;
	vector<Point2f> retrackedPoints;
	vector<Point2f> inliers;

	Point2f tl = r.tl();

	for (x = 1; x <= DIM_POINTS; x++) {
		for (y = 1; y <= DIM_POINTS; y++) {
			Point2f selectedPoint = tl + Point2f(x*stepX, y*stepY);
			startPoints.push_back(selectedPoint);						
		}
	}

	reverse(frames.begin(), frames.end());	
	trackForeward(startPoints, frames, &endPoints);
	reverse(frames.begin(), frames.end());
	
	trackForeward(endPoints, frames, &retrackedPoints);

	filterInliers(startPoints, retrackedPoints, &inliers);

	Mat c = frames[0].clone();
	drawPoints(inliers, c);
	imshow("inliers",c);
	cv::waitKey();

	int inliersSize = inliers.size();
	float* dxs =  new float[inliersSize];
	float* dys =  new float[inliersSize];
	int successful = 0;

	Point2f* distances = new Point2f[inliersSize];
	for(int i = 0 ; i < inliersSize; i++)
	{
		distances[i] = startPoints[i] - retrackedPoints[i];		
	}

	// Get the median displacements
	double dispX = (double)median(dxs, successful);
	double dispY = (double)median(dys, successful);

	delete[] distances;
}

void BlobTracker::injectBlobDescription( blob* b, Mat image, Mat foreground )
{
	Mat subimage = image(b->sourceRect);	
	vector<KeyPoint> keypoints;
	
	Mat descriptors;	
	Mat mask = foreground;
	CV_Assert( mask.empty() || (mask.type() == CV_8UC1 && mask.size() == image.size()) );
	this->featureDetector->detect(subimage, keypoints, Mat());
	this->descriptorExtractor->compute(subimage, keypoints, descriptors);
	
	Mat img_keypoint;
	cv::drawKeypoints( subimage, keypoints, img_keypoint, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	imshow("bl", subimage);
	imshow("kps", img_keypoint);

	//cv::waitKey();

	b->descriptor.descriptor = descriptors;
	b->descriptor.keypoints = keypoints;
	b->descriptor.image = image(b->sourceRect).clone();
}







void BlobTracker::trackForeward(vector<Point2f> points, vector<Mat> frames, vector<Point2f>* result)
{
	assert(frames.size() > 1);
	assert(result != NULL);

	result->clear();

	int count = frames.size() - 1;
	result->clear();

	vector<Point2f> points_t = points;
	vector<Point2f> points_t1;
	vector<uchar> status;
	vector<float> errors;
	for(int i = 0; i < count ; i++)
	{
		points_t1.clear();

		imshow("f", frames[i]);
		waitKey();

		cv::calcOpticalFlowPyrLK(
			frames[i],			
			frames[i+1], 				// 2 consecutive images, must be gray
			points_t, 	// input point position in first image
			points_t1, 	// output point postion in the second image
			status,			// tracking success
			errors			// tracking error  	   
			);  

		status.clear();
		errors.clear();

		Mat c1 = frames[i].clone();
		drawPoints(points_t, c1);
		imshow("t", c1);
		cv::waitKey();

		Mat c2 = frames[i+1].clone();
		drawPoints(points_t1, c2);			
		imshow("t", c2);
		cv::waitKey();

		points_t = points_t1;
	}

	*result = points_t1;
}

void BlobTracker::filterInliers(vector<Point2f> startPoints, vector<Point2f> backTrackedPoints, vector<Point2f>* result)
{
    assert(result != NULL);
	assert(startPoints.size() == backTrackedPoints.size());
	result->clear();

	float treshold = 2;

	for(int i = 0; i < startPoints.size(); i++)
	{
		Point2f pf = startPoints[i];
		Point2f pb = backTrackedPoints[i];

		float xdif = abs(pf.x - pb.x);
		float ydif = abs(pf.y - pb.y);

		if(sqrt(xdif*xdif + ydif*ydif) < 1)
		{
			result->push_back(pf);			
		}
	}
}