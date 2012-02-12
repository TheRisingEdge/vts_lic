#pragma once
#include <opencv2/opencv.hpp>
#include <map>
#include "Content.h"
#include "AppConfig.h"

using namespace cv;
using namespace std;

class Importer
{
	private:		
		 vector<Mat> carSamples;
		 vector<Mat> nonCarSamples;
		 bool carsLoaded;
		 bool nonCarsLoaded;

	public:
		Importer(void);
		~Importer(void);
			
		char* videoPath(int nr);
		Mat loadTestImage(int nr);
		Mat loadCarImage(int nr);
		Mat loadNonCarSample(int nr);
		void loadTrainingImages();
		Mat loadGrayImage(char* path);
		vector<Mat> loadCarImages();
		vector<Mat> loadNonCarImages();
};
