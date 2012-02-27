#pragma once
#include "opencv2\legacy\blobtrack.hpp"
#include "opencv2\imgproc\imgproc_c.h"
#include "opencv2\core\core.hpp"

#define MIN_HEIGHT 10
#define MIN_WIDTH  10

class BlobDetector:public CvBlobDetector
{
private:	
	int minArea;

public:
    BlobDetector();
   ~BlobDetector();    
    int DetectNewBlob(IplImage* frame, IplImage* fgMask, CvBlobSeq* potentialBlobList, CvBlobSeq* trackedBlobs);
    void Release();
};