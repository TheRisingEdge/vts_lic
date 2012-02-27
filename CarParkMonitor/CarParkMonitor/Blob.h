#pragma once
#include "opencv2/opencv.hpp"
#include <map>

using namespace cv;
using namespace std;

typedef struct
{
	Mat descriptor;

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

class TrackHistory
{
private:
	vector<blob> history;
	int size;

public:
	TrackHistory(int size)
	{
		this->size = size;
	}

	void append(blob b)
	{
		/*vector<int>::iterator it = history.begin();
		myvector.insert ( it , b );		
		history.resize(size);	*/	
	}
};

