#pragma once
#include "opencv2\core\core.hpp"
#include <vector>
#include "Blob.h"

using namespace std;

class TrackHistory
{
private:
	vector<blob> history;
	int size;

public:
	BlobSeq* prevFrameBlobs;
	vector<blob> previousBlobs;

	TrackHistory(int size)
	{
		this->size = size;
	}

	void append(blob b)
	{

	}

	void update();
};