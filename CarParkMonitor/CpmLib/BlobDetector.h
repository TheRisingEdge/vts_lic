#pragma once
#include "opencv2/opencv.hpp"
#include "Blob.h"

using namespace cv;

#define BLOB_DRAW 0

typedef struct
{
	Mat frame;
	Mat foreground;
	Mat prevFrame;
	bool filterToRect;
	Rect filterRect;
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
	BlobDetector(int minWidth = 15, int minHeight = 20);
	~BlobDetector(){}

	vector<shared_ptr<blob>> detect(DetectorParams params);
};