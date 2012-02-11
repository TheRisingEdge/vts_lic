#pragma once
#include "opencv2/opencv.hpp"

using namespace  cv;

class SvmComponent
{
private:	
	CvSVM svm;	
	CvSVMParams getSvmParams();

public:
	SvmComponent(void);
	~SvmComponent(void);
		
	void train(Mat_<float> positiveSamples, Mat_<float> negativeSamples);
	float predict(Mat sample);
	void save();
	void load();
};

