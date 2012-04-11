#include "BlobDetector.h"
#include "opencv2/opencv.hpp"
#include <memory>
#include "Tool.h"

using namespace cv;
static const int MARGIN = 3;

int BlobDetector::instanceNr = 1;

void BlobDetector::init(int minWidth, int minHeight, char* windowName )
{
	instanceNr++;
	this->minWidth = minWidth;
	this->minHeight = minHeight;
	this->minArea = minWidth*minHeight;
	this->windowName = windowName;
}

BlobDetector::BlobDetector(int minWidth, int minHeight, char* windowName)
{
	init(minWidth, minHeight, windowName);
}

BlobDetector::BlobDetector(int minWidth, int minHeight)
{
	char* windowName = new char[50];
	sprintf(windowName, "detector %d", instanceNr);

	init(minWidth, minHeight, windowName);
}

vector<blob> BlobDetector::detect( DetectorParams params )
{
	Mat foreground = params.foreground;
	Mat frame 	   = params.frame;
	Mat maskCopy   = foreground.clone();

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( maskCopy, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );		

	vector<blob> foundBlobs;	
	int size = contours.size();
	for( int i = 0; i < size; i++ )
	{     
		Rect rect = boundingRect( Mat(contours[i]) );		
		Point tl = rect.tl();
		Point br = rect.br();

		if(rect.area() > this->minArea &&
			rect.width > minWidth  && rect.height > minHeight &&
			tl.x > MARGIN && tl.y > MARGIN &&
			(br.x < (frame.cols - MARGIN)) && (br.y < (frame.rows - MARGIN)))
		{		
			blob b;
			b.rect = rect;
			b.detectorId = i;
			b.contour = contours[i];
			b.id = ID_UNDEFINED;					
			foundBlobs.push_back(b);					
		}			
	}

	maskCopy.release();

#if BLOB_DRAW
	Mat cl = frame.clone();
	RNG rng(12345);	
	for_each(begin(foundBlobs), end(foundBlobs), [&](blob& b)
	{
		Rect rect = b.rect;
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		rectangle( cl, rect.tl(), rect.br(), color);
	});
	imshow(this->windowName, cl );
	cl.release();
#endif
	
	return foundBlobs;
}

