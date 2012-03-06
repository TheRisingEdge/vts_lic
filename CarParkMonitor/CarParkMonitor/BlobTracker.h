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

}MatcherParams;

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
	void match(MatcherParams params, MatcherResult* result);
};