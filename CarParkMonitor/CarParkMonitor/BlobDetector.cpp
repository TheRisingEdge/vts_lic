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

void BlobDetector::detect(DetectorParams params , vector<blob>* foundBlobs )
{
	Mat foreground = params.foreground;
	Mat frame = params.frame;
	Mat maskCopy = foreground.clone();

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( maskCopy, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	foundBlobs->clear();
	int size = contours.size();
	for( int i = 0; i < size; i++ )
	{     
		Rect rect = boundingRect( Mat(contours[i]) );		
		if(rect.area() > this->minArea && rect.width > MIN_WIDTH)
		{			
			blob b;
			b.sourceRect = rect;
			foundBlobs->push_back(b);
		}			
	}

#pragma region debug_draw

	RNG rng(12345);	
	size = foundBlobs->size();
	for( int i = 0; i< size; i++ )
	{
		Rect rect = (*foundBlobs)[i].sourceRect;
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		rectangle( frame, rect.tl(), rect.br(), color);
	}

	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	imshow( "Contours", frame );

#pragma endregion debug_draw
}
