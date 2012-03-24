#pragma once
#include <opencv2\opencv.hpp>
#include <memory>
#include "Blob.h"


using namespace cv;
using namespace std;

typedef struct
{
	float centerX;
	float centerY;

	float width;
	float height;

	float vx;
	float vy;

	Rect toRect()
	{
		Rect r(centerX - width/2, centerY - height/2, width, height);
		return r;
	}

}EstimatorResult;

class KalmanEstimator
{
private:
	unique_ptr<KalmanFilter> filter;	
	cv::Mat_<float> measurement;
	EstimatorResult lastPrediction;

public:
	int blodId;

	KalmanEstimator(void);
	~KalmanEstimator(void);

	void init(shared_ptr<blob> blob);
	EstimatorResult predict();
	EstimatorResult correct(shared_ptr<blob> b);
	EstimatorResult update(shared_ptr<blob> b);
	EstimatorResult getLastPrediction();
};
