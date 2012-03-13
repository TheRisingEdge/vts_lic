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

vector<blob*> BlobDetector::detect( DetectorParams params )
{
	Mat foreground = params.foreground;
	Mat frame 	   = params.frame;
	Mat maskCopy   = foreground.clone();

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( maskCopy, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );		

	vector<blob*> foundBlobs;	
	int size = contours.size();
	for( int i = 0; i < size; i++ )
	{     
		Rect rect = boundingRect( Mat(contours[i]) );		
		Point tl = rect.tl();
		if(rect.area() > this->minArea && rect.width > MIN_WIDTH&& tl.x > 20 && tl.y > 20)
		{			
			blob* b = new blob;
			b->rect = rect;
			b->detectorId = i;
			b->id = ID_UNDEFINED;
			b->temp.init();

			foundBlobs.push_back(b);
		}			
	}

#pragma region debug_draw

	Mat cl = frame.clone();
	RNG rng(12345);	
	size = foundBlobs.size();
	for( int i = 0; i< size; i++ )
	{
		Rect rect = foundBlobs[i]->rect;
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		rectangle( cl, rect.tl(), rect.br(), color);
	}


	namedWindow( "Blob detector", CV_WINDOW_AUTOSIZE );
	imshow( "Blob detector", cl );
	cl.release();

#pragma endregion debug_draw
	
	return foundBlobs;
}
