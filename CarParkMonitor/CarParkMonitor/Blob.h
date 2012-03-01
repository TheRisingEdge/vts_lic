#pragma once
#include "opencv2/opencv.hpp"
#include <map>

using namespace cv;
using namespace std;

typedef struct
{
	Mat descriptor;
	vector<KeyPoint> keypoints;
	vector<Point2f> pointFeatures;

}blobDescriptor;

typedef struct
{
	int id;
	blobDescriptor descriptor;
	Rect sourceRect;
	
}blob;

class IdGenerator
{
	int current;

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
			b.sourceRect = rects[i];
			addBlob(b);
		}
	}

	void addBlob(Rect r)
	{
		blob b;
		b.sourceRect = r;
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


