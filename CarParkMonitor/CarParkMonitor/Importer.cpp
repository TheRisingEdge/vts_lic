#include "Importer.h"

vector<Mat> Importer::carSamples;
vector<Mat> Importer::nonCarSamples;

Importer::~Importer(void)
{

}

Importer::Importer( void )
{

}

void  Importer::loadSamples()
{					
	int carCount			=  AppConfig::carSampleCount;
	int nonCarSampleCount	=  AppConfig::nonCarSampleCount;

	for(int i = 0 ; i < carCount; i++)
	{							
		Mat image = loadCarSample(i);
		Importer::carSamples.push_back(image);		
	}					

	for(int i = 0; i < nonCarSampleCount; i++)
	{							
		Mat image = loadNonCarSample(i);
		Importer::nonCarSamples.push_back(image);
	}
}

vector<Mat> Importer::getNonCarSamples()
{
	return Importer::nonCarSamples;
}

vector<Mat> Importer::getCarSamples()
{
	return Importer::carSamples;
}

cv::Mat Importer::loadNonCarSample( int nr )
{
	char* path = Content::pathTo("./Content/TrainImages/neg-%d.pgm", nr);	
	return imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

cv::Mat Importer::loadCarSample( int nr )
{
	char* path = Content::pathTo("./Content/TrainImages/pos-%d.pgm", nr);	
	return imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

cv::Mat Importer::loadTestSample( int nr )
{
	char* path = Content::pathTo("./Content/TestImages/test-%d.pgm",nr);
	return imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

cv::Mat Importer::loadGrayImage( char* fileName )
{
	char* path = new char[100];
	sprintf(path, "./Content/Images/%s", fileName);
	return imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}
