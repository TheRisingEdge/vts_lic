#pragma once
#include "opencv2/opencv.hpp"
#include <map>

using namespace cv;
using namespace std;


#define ID_UNDEFINED -1

typedef struct
{
	int id;
	int detectorId;		
	Rect rect;	
	vector<Point> contour;

	//vector<int> inferedIds;
	//vector<Point2f> trackedPoints;
}blob;

typedef struct
{
	int id;
	int blobId;
	Rect rect;

}carDetection;

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
		current++;
		return current;
	}

	void releaseId(int id){}
};