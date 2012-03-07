#pragma once
#include "Blob.h"
#include "TrackHistory.h"

#define NO_MATCH -1

typedef struct
{
	int frameCount;

	Mat frame;
	Mat grayFrame;	
	Mat foreground;
	
	Mat previousFrame;
	Mat previousGrayFrame;
	Mat previousForeground;
				
	IdGenerator* generator;
	vector<blob*> detectedBlobs;

	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;

}TrackerParam;

typedef struct
{
	vector<blob*> newBlobs;
	vector<blob*> prevLostBlobs;

	void init()
	{
		newBlobs.clear();
		prevLostBlobs.clear();
	}

}MatcherResult;

class BlobTracker
{
private:
	TrackHistory* trackHistory;
	FeatureDetector* featureDetector;
	DescriptorExtractor* descriptorExtractor;

public:
	BlobTracker(void);
	BlobTracker(TrackHistory* trackHistory);
	~BlobTracker(void);	

	void injectBlobDescription(blob* b, Mat image, Mat foreground);
	void match(TrackerParam params, MatcherResult* result);
	void track(Rect r, vector<Mat> frames);	
};