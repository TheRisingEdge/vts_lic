#pragma once
#include "ClassifierBase.h"
#include <opencv2\gpu\gpu.hpp>
#include "PngSaver.h"
using namespace cv::gpu;

class HogClassifier: public ClassifierBase
{
private:
	Mat readHPlane(char* path = "./Content/Assets/hogsvm.yml");
	void load();
	gpu::GpuMat gpuMat;
	gpu::HOGDescriptor hogGpu;
	
public:
	HogClassifier(){
		load();
		//PngSaver::setBasePath("./Content/Images");
	};

	~HogClassifier(){};

	vector<detection> detect(ClassifierParams& params);
};

