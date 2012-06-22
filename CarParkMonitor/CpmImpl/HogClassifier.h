#pragma once
#include "ClassifierBase.h"
#include <opencv2\gpu\gpu.hpp>
#include "PngSaver.h"
using namespace cv::gpu;

class HogDetector: public ClassifierBase
{
private:
	Mat readHPlane(char* path = "./Content/Assets/hogsvm0.yml");
	void load();
	gpu::GpuMat gpuMat;
	gpu::HOGDescriptor hogGpuDetector;
	
public:
	HogDetector(){
		load();
		//PngSaver::setBasePath("./Content/Images");
	};

	~HogDetector(){};

	vector<detection> detect(ClassifierParams& params);
};

