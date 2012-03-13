#include "BlobTracker.h"
#include <assert.h>
#include "BlobDetector.h"
#include "AppConfig.h"
#include <algorithm>


using namespace std;

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
		Rect rect = blobs.at(i)->rect;

		rectangle( output, rect.tl(), rect.br(), color);

		stringstream id;//create a stringstream
		id << blobs.at(i)->id;//add number to the stream

		string text = id.str();
				
		cv::Point textOrg = blobs.at(i)->rect.tl();
		cv::putText(output, text, textOrg, fontFace, fontScale, red, thickness,8);
	}
}

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

bool isSameBlob(blob* a, blob* b)
{
	return true;
}

void BlobTracker::track( TrackerParam param, MatcherResult* result )
{
	this->trackerParam = param;

	int frameCount 				= param.frameCount;
	int frameBufferSize			= param.frameBufferSize;

	Mat frame 					= param.frameBuffer[0];
	Mat foreground				= param.foregroundBuffer[0];
	Mat grayFrame 	  			= param.grayFrameBuffer[0];
	vector<blob*> detectedBlobs = param.blobBuffer[0];
	
	Mat prevFrame 				= param.prevFrame;
	Mat prevForeground 			= param.prevForeground;	
	Mat prevGrayFrame 			= param.prevGrayFrame;

	vector<blob*> prevBlobs 	= trackHistory->previousBlobs;	
	if(prevBlobs.size() == 0 && detectedBlobs.size() == 0)	
	{
		return;
	}

	if(prevBlobs.size() == 0 && detectedBlobs.size() != 0)
	{
		result->newBlobs.assign(detectedBlobs.begin(), detectedBlobs.end());
		return;
	}
	
	int prevSize = prevBlobs.size();
	int currentSize = detectedBlobs.size();
	
	for_each(begin(prevBlobs), end(prevBlobs), [&](blob* b){
		assert(b->id != ID_UNDEFINED);



	});




	//vector<blob*> allBlobs;
	//allBlobs.assign(prevBlobs.begin(), prevBlobs.end());
	//allBlobs.insert(allBlobs.end(), detectedBlobs.begin(), detectedBlobs.end());

	//vector<int> labels;
	//cv::partition(allBlobs,labels, isSameBlob);
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

	
	forewardTrack(startPoints, frames, &endPoints);
	
	reverse(frames.begin(), frames.end());	
	forewardTrack(endPoints, frames, &retrackedPoints);
	reverse(frames.begin(), frames.end());

	filterInliers(startPoints, retrackedPoints, inliers);

	Mat c = frames[0].clone();
	drawPoints(inliers, c);
	imshow("inliers",c);
	cv::waitKey();

	//int inliersSize = inliers.size();
	//float* dxs =  new float[inliersSize];
	//float* dys =  new float[inliersSize];
	//int successful = 0;

	//Point2f* distances = new Point2f[inliersSize];
	//for(int i = 0 ; i < inliersSize; i++)
	//{
	//	distances[i] = startPoints[i] - retrackedPoints[i];		
	//}

	//// Get the median displacements
	//double dispX = (double)median(dxs, successful);
	//double dispY = (double)median(dys, successful);

	//delete[] distances;
}

void BlobTracker::forewardTrack(vector<Point2f> points, vector<Mat> frames, vector<Point2f>* result)
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

		Mat c2 = frames[i+1].clone();
		drawPoints(points_t1, c2);			
		imshow("t", c2);
		cv::waitKey();
		c2.release();

		points_t = points_t1;
	}

	*result = points_t1;
}

void BlobTracker::filterInliers(vector<Point2f> startPoints, vector<Point2f> backTrackedPoints, vector<Point2f>& result)
{
	assert(startPoints.size() == backTrackedPoints.size());
	result.clear();

	float treshold = 1;
	
	for(int i = 0; i < startPoints.size(); i++)
	{
		Point2f pf = startPoints[i];
		Point2f pb = backTrackedPoints[i];

		float xdif = abs(pf.x - pb.x);
		float ydif = abs(pf.y - pb.y);

		float dist = xdif*xdif + ydif*ydif;
		if( dist < treshold)
		{
			result.push_back(pf);			
		}
	}
}