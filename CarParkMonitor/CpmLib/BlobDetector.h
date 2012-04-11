#pragma once
#include "opencv2/opencv.hpp"
#include "Blob.h"

using namespace cv;

#define BLOB_DRAW 1

struct DetectorParams
{
	Mat frame;
	Mat foreground;
	Mat prevFrame;
};

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
	BlobDetector(int minWidth = 15, int minHeight = 20);
	~BlobDetector(){}

	vector<blob> detect(DetectorParams params);
};