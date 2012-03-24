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
	vector<shared_ptr<vehicleDetection>> detections;

}ClassifierResult;

class VehicleClassifier
{
public:
	VehicleClassifier(void);
	~VehicleClassifier(void);

	float detectVehicle(const Mat& image,const Mat& mask);
	void detect(const Mat& image, vector<Rect>& detections);

	void detect(const ClassifierParam& param, ClassifierResult& result);
};

