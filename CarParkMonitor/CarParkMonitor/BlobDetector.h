#pragma once
#include "opencv2/opencv.hpp"
#include "Blob.h"
#include "BlobTracker.h"

using namespace cv;
#define MIN_HEIGHT 10
#define MIN_WIDTH  20

#define BLOB_DRAW 1

typedef struct
{
	Mat frame;
	Mat foreground;
	Mat prevFrame;

}DetectorParams;

class BlobDetector
{
private:	
	int minArea;
	BlobTracker* blobMatcher;

public:
    BlobDetector();
   ~BlobDetector();

	vector<shared_ptr<blob>> detect(DetectorParams params);
};