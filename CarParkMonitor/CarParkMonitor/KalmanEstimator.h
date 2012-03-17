#pragma once
#include "opencv2\video\tracking.hpp"
#include <memory>

using namespace cv;
using namespace std;

class KalmanEstimator
{

	unique_ptr<KalmanFilter> filter;

public:
	KalmanEstimator(void);
	~KalmanEstimator(void);

	//void correct();
	//void update();
	//void predict();
};

