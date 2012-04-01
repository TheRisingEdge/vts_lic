#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;
#include <opencv2\opencv.hpp>
#include "Loader.h"
#include "Content.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Loader::setBasePath("./../CarParkMonitor/Content/Images");

	static const int width = 80;
	static const int height = 64;
	static const int fvectorSize = 2268;

	HOGDescriptor hog(
		Size(width, height),//images size
		Size(16,16),//block size
		Size(8,8),//block stride
		Size(8,8),//cell size
		9
	);
	
	ofstream train;
	train.open("./../CarParkMonitor/Content/Assets/hogtrain.txt");		
	train.clear();

	auto positives = Loader::getPositiveSamples();	
	for_each(begin(positives), end(positives), [&](const Mat& image){
		vector<float> descriptor;
		descriptor.reserve(fvectorSize);
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
		vector<float> descriptor;
		descriptor.reserve(fvectorSize);
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