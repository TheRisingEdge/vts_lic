#pragma once
#include "Blob.h"
#include <opencv2/opencv.hpp>

using namespace cv;

struct ClassifierParams
{
	Mat frame;	
};

struct detection
{
	int id;
	Rect rect;
};

class ClassifierBase
{
public:
	ClassifierBase(){};
	~ClassifierBase(){};

	virtual vector<detection> detect(ClassifierParams& params) = 0;
};

