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
		 vector<Mat> allCars;
		 vector<Mat> allNonCars;
		 vector<Mat> trainingCars;
		 vector<Mat> testCars;

		 bool allCarsLoaded;
		 bool nonCarsLoaded;
		 bool testCarsLoaded;

	public:
		Importer(void);
		~Importer(void);
			
		char* videoPath(int nr);

		Mat loadTestImage(int nr);
		Mat loadCarImage(int nr);
		Mat loadNonCarSample(int nr);		
		Mat loadGrayImage(char* path);

		void loadAllImages();

		vector<Mat> loadCarImages();
		vector<Mat> loadNonCarImages();
		vector<Mat> loadTestImages();
};
