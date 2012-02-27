#pragma once
#include "Blob.h"

#define NO_MATCH -1

class BlobMatcher
{
public:
	BlobMatcher(void);
	~BlobMatcher(void);	

	void match(blob queryBlob, BlobSeq targetBlobs, vector<float>* matches);
	void match(BlobSeq queryBlobs, BlobSeq targetBlobs, vector<vector<float>>* matches);
	void match(vector<blob> queryBlobs, BlobSeq targetBlobs, vector<int>* matches, Mat* currentFrame = NULL, Mat* previousFrame = NULL);
};

