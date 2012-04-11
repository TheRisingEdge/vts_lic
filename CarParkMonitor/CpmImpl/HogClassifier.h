#pragma once
#include "ClassifierBase.h"
#include <opencv2\gpu\gpu.hpp>

using namespace cv::gpu;

class HogClassifier: public ClassifierBase
{
private:
	Mat readHPlane();
	void load();
	gpu::GpuMat gpuMat;
	gpu::HOGDescriptor hogGpu;
	
public:
	HogClassifier(){
		load();
	};

	~HogClassifier(){};

	vector<detection> detect(ClassifierParams& params);
};

