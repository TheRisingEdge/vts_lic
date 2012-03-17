#include "KalmanEstimator.h"


KalmanEstimator::KalmanEstimator(void)
{
	filter = unique_ptr<KalmanFilter>(new KalmanFilter(4,2));
}


KalmanEstimator::~KalmanEstimator(void)
{
}
