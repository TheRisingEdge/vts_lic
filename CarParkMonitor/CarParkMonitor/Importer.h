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
		static vector<Mat> carSamples;
		static vector<Mat> nonCarSamples;

	public:
		Importer(void);
		~Importer(void);
			
		static char* videoPath(int nr)
		{
			return Content::pathTo("./Content/Videos/video-%d.avi", nr);					
		}
		static Mat loadTestSample(int nr);
		static Mat loadCarSample(int nr);
		static Mat loadNonCarSample(int nr);
		static void loadSamples();
		static vector<Mat> getCarSamples();
		static vector<Mat> getNonCarSamples();
		static Mat loadGrayImage(char* path);
};
