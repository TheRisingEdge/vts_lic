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
	vector<int> inferedIds;

	vector<Point2f> trackedPoints;

}blob;

typedef struct
{
	int blobId;
	Rect rect;

}vehicleDetection;

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

class BlobSeq
{
private:
	IdGenerator* generator;
	map<int, blob> idMap;
	int nextId;

public:
	BlobSeq(IdGenerator* generator)
	{
		this->generator = generator;
	}

	BlobSeq(vector<Rect> rects)
	{
		int size = 0;
		for(int i = 0; i < size; i++)
		{
			blob b;
			b.rect = rects[i];
			addBlob(b);
		}
	}

	void addBlob(Rect r)
	{
		blob b;
		b.rect = r;
		b.id = nextId;
		nextId++;

		addBlob(b);
	}

	void addBlob(blob b){		
		idMap[b.id] = b;
	}

	void deleteBlob(blob b)
	{
		idMap.erase(b.id);
	}

	blob* findById(int id)
	{
		return &(idMap[id]);
	}	

	blob* next()
	{
		return NULL;
	}
};

struct historyEntry
{
	int blobId;
};


