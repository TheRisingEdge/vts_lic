#include "BlobDetector.h"
#include "opencv2/opencv.hpp"

using namespace cv;


BlobDetector::BlobDetector()
{
	this->minArea = MIN_HEIGHT*MIN_WIDTH;	
}

BlobDetector::~BlobDetector()
{
}

void BlobDetector::process( Mat frame, Mat foreground, BlobSeq trackedBlobs, BlobSeq* newBlobs )
{
	Mat maskCopy = foreground.clone();

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( maskCopy, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	vector<Rect> boundingRectangles;
	vector<blob> detected;
	int size = contours.size();
	for( int i = 0; i < size; i++ )
	{     
		Rect rect = boundingRect( Mat(contours[i]) );		
		if(rect.area() > this->minArea && rect.width > MIN_WIDTH)
		{
			//boundingRectangles.push_back(rect);
			blob b;
			b.sourceRect = rect;
			detected.push_back(b);
		}			
	}

#pragma region debug_draw

	RNG rng(12345);	
	size = boundingRectangles.size();
	for( int i = 0; i< size; i++ )
	{
		Rect rect = boundingRectangles[i];
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		rectangle( frame, rect.tl(), rect.br(), color);
	}

	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	imshow( "Contours", frame );

#pragma endregion debug_draw

	//vector<int> matches;
	//blobMatcher->match(detected, trackedBlobs, &matches, &frame);

	//size = matches.size();
	//for(int i = 0; i < size; i++)
	//{
	//	if(matches[i] == NO_MATCH)
	//	{
	//		trackedBlobs.addBlob(detected[i]);
	//	}
	//}





}
