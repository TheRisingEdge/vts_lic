#include "KalmanEstimator.h"
#include "Tool.h"

/*======================= KALMAN FILTER =========================*/
/* State vector is (x,y,w,h,dx,dy,dw,dh). */
/* Measurement is (x,y,w,h). */


#define F \
	1, 0, 0, 0, 1, 0, 0, 0, \
	0, 1, 0, 0, 0, 1, 0, 0, \
	0, 0, 1, 0, 0, 0, 1, 0, \
	0, 0, 0, 1, 0, 0, 0, 1, \
	0, 0, 0, 0, 1, 0, 0, 0,	\
	0, 0, 0, 0, 0, 1, 0, 0,	\
	0, 0, 0, 0, 0, 0, 1, 0,	\
	0, 0, 0, 0, 0, 0, 0, 1

/* Measurement matrix H: */
#define H \
	1, 0, 0, 0, 0, 0, 0, 0, \
	0, 1, 0, 0, 0, 0, 0, 0,	\
	0, 0, 1, 0, 0, 0, 0, 0,	\
	0, 0, 0, 1, 0, 0, 0, 0


KalmanEstimator::KalmanEstimator(void)
{
	filter = unique_ptr<KalmanFilter>(new KalmanFilter(8,4));

	//setup error covariances
	setIdentity(filter->processNoiseCov, Scalar::all(1e-4));
	setIdentity(filter->measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(filter->errorCovPost, Scalar::all(.1));

	//setup transition matrix with F
	filter->transitionMatrix = (Mat_<float>(8,8) << F );
								
	//setup measurement matrix with H
	filter->measurementMatrix = (Mat_<float>(4,8) << H);

	measurement = Mat_<float>(4,1);

	lastPrediction.centerX = 0;
	lastPrediction.centerY = 0;
	lastPrediction.width   = 0;
	lastPrediction.height  = 0;
	lastPrediction.vx 	   = 0;
	lastPrediction.vy 	   = 0;
}


KalmanEstimator::~KalmanEstimator(void)
{
}

void KalmanEstimator::init( shared_ptr<carDetection> b )
{
	this->vehicleId = b->id;

	auto size = b->rect.size();
	auto center = Tool::rectCenter(b->rect);
		
	filter->statePost.at<float>(0) = center.x;
	filter->statePost.at<float>(1) = center.y;
	filter->statePost.at<float>(2) = size.width;
	filter->statePost.at<float>(3) = size.width;
	filter->statePost.at<float>(4) = 0;//dx
	filter->statePost.at<float>(5) = 0;//dy
	filter->statePost.at<float>(6) = 0;//wx
	filter->statePost.at<float>(7) = 0;//wy
}

inline void mapMatToResult(const Mat& m, EstimatorResult& result)
{
	result.centerX = m.at<float>(0);
	result.centerY = m.at<float>(1);
	result.width   = m.at<float>(2);
	result.height  = m.at<float>(3);	
}

EstimatorResult KalmanEstimator::correct( shared_ptr<carDetection> b )
{
	assert(this->vehicleId == b->id);

	auto size = b->rect.size();
	auto center = b->rect.tl()+ Point(size.width/2, size.height/2);
	
	measurement(0) = center.x;
	measurement(1) = center.y;
	measurement(2) = size.width;
	measurement(3) = size.height;

	//apply measurement
	Mat estimated = filter->correct(measurement);
	
	EstimatorResult result;
	mapMatToResult(estimated, result);
	return result;
}

EstimatorResult KalmanEstimator::predict()
{
	Mat estimated = filter->predict();

	EstimatorResult result;
	mapMatToResult(estimated, result);
	lastPrediction = result;

	return result;
}

EstimatorResult KalmanEstimator::update( shared_ptr<carDetection> b )
{
	auto prediction = this->predict();
	this->correct(b);
	return prediction;
}

EstimatorResult KalmanEstimator::getLastPrediction()
{
	return lastPrediction;
}