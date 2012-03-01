#pragma once
#include "Blob.h"
#include "TrackHistory.h"

#define NO_MATCH -1

typedef struct
{
	Mat frame;
	Mat foreground;

	Mat previousFrame;
	Mat previousForeground;
	
	vector<blob>* detectedBlobs;

}MatcherParams;

typedef struct
{

}MatcherResult;

class BlobTracker
{
private:
	TrackHistory* trackHistory;

public:
	BlobTracker(void);
	BlobTracker(TrackHistory* trackHistory);
	~BlobTracker(void);	

	void match(MatcherParams params, MatcherResult* result);
};

