#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>

#include "Loader.h"

using namespace cv;
using namespace std;

struct RunConfig
{
	int width;
	int height;
};

RunConfig readLastConfig()
{
	RunConfig c;

	//FileStorage f = FileStorage("./../CarParkMonitor/Content/Assets/lastExtractionConfig.yml", FileStorage::READ);
	//f["width"] >> c.width;
	//f["height"] >> c.height;
	//f.release();

	c.width = 80;
	c.height = 64;
	return c;
}

int main(int argc, char** argv)
{

	RunConfig cfg = readLastConfig();
	static const int width = cfg.width;
	static const int height = cfg.height;

	static const int fvectorSize = 2268;

	cv::HOGDescriptor hog(
		Size(width, height),//images size
		Size(16,16),//block size
		Size(8,8),//block stride
		Size(8,8),//cell size
		9
	);

	Loader::setPosPathAndCount("./../CarParkMonitor/Content/DatasetExtractions/ext1", 1000);
	Loader::setNegPathAndCount("./../CarParkMonitor/Content/DatasetExtractions/ext1",1000);

	ofstream train;
	train.open("./../CarParkMonitor/Content/Assets/evo1_80x64.txt");		
	train.clear();

	vector<float> descriptor;
	//descriptor.reserve(fvectorSize);

	auto positives = Loader::getPositiveSamples();	
	for_each(begin(positives), end(positives), [&](const Mat& image){
		descriptor.clear();

		hog.compute(image, descriptor);

		train << "+1";
		int size = descriptor.size();
		for(int i = 0; i < size; i++)
		{
			int findex = i+1;
			train <<" "<< findex <<":"<<descriptor[i];
		}
		train << endl;
	});

	auto negatives = Loader::getNegativeSamples();
	for_each(begin(negatives), end(negatives), [&](const Mat& image){
		descriptor.clear();

		hog.compute(image, descriptor);

		train << "-1";
		int size = descriptor.size();
		for(int i = 0; i < size; i++)
		{
			int findex = i+1;
			train <<" "<< findex <<":"<<descriptor[i];
		}
		train << endl;
	});

	train.close();			
	return 0;
}