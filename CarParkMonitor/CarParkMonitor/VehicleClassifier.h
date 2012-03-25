#pragma once
#include <opencv2/opencv.hpp>
#include "Blob.h"

using namespace cv;

typedef struct
{
	Mat frame;
	Mat foreground;
	vector<shared_ptr<blob>> blobBuffer;

}ClassifierParam;

typedef struct
{
	vector<shared_ptr<carDetection>> detections;

}ClassifierResult;

class VehicleClassifier
{
public:
	VehicleClassifier(void);
	~VehicleClassifier(void);

	vector<shared_ptr<carDetection>> detect(const ClassifierParam& param);
};

