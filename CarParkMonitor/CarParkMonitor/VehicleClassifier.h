#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class VehicleClassifier
{
public:
	VehicleClassifier(void);
	~VehicleClassifier(void);

	float detectVehicle(const Mat& image,const Mat& mask);
	void detect(const Mat& image, vector<Rect>& detections);
};

