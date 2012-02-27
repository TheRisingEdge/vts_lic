#pragma once
#include "opencv2/opencv.hpp"
#include "Blob.h"
#include "BlobMatcher.h"

using namespace cv;

#define MIN_HEIGHT 10
#define MIN_WIDTH  20


class BlobDetector
{
private:	
	int minArea;
	BlobMatcher* blobMatcher;

public:
    BlobDetector();
   ~BlobDetector();

	void process(Mat frame, Mat foreground, BlobSeq trackedBlobs, BlobSeq* newBlobs);
};