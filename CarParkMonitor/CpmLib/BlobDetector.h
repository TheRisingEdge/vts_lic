#pragma once
#include "opencv2/opencv.hpp"
#include "Blob.h"

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
protected:
	char* windowName;
	static int instanceNr;
	void init(int minWidth, int minHeight, char* windowName);

private:	
	int minArea;
	int minWidth;
	int minHeight;

public:
    BlobDetector(int minWidth = 15, int minHeight = 20, char* windowName = "detector");
   ~BlobDetector();

	vector<shared_ptr<blob>> detect(DetectorParams params);
};