#pragma once
#include "opencv2/opencv.hpp"
#include <map>

using namespace cv;
using namespace std;


#define ID_UNDEFINED -1

struct blob
{
	int id;
	int detectorId;		
	Rect rect;	
	vector<Point> contour;
};

struct carDetection
{
	int id;
	int blobId;
	Rect rect;
};

class IdGenerator
{
	int current;

public:
	IdGenerator(int base)
	{
		current = base;
	}

	int nextId()
	{
		int next = current++;
		return next;
	}

	void releaseId(int id){}
};