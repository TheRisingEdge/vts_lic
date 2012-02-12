#include "Importer.h"

Importer::~Importer(void)
{
}

Importer::Importer( void )
{
	nonCarsLoaded = false;
	carsLoaded = false;
}

void loadImagesWithIndexPath(char* indexPath, int count, vector<Mat>* output)
{
	for(int i = 1; i < count; i++)
	{
		char* path = Content::pathTo(indexPath, i);	
		Mat image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
		output->push_back(image);
	}
}

vector<Mat> Importer::loadCarImages()
{
	if(!carsLoaded)
	{
		int carCount	=  AppConfig::carSampleCount;	
		loadImagesWithIndexPath("./Content/TrainImages/pos-%d.pgm", carCount, &carSamples);		
		carsLoaded = true;
	}
	
	return carSamples;
}

vector<Mat> Importer::loadNonCarImages()
{
	if(!nonCarsLoaded)
	{
		int nonCarCount	=  AppConfig::nonCarSampleCount;
		loadImagesWithIndexPath("./Content/TrainImages/neg-%d.pgm", nonCarCount, &nonCarSamples);	
		nonCarsLoaded = true;
	}

	return nonCarSamples;
}

void  Importer::loadTrainingImages()
{						
	loadCarImages();	
	loadNonCarImages();
}

cv::Mat Importer::loadNonCarSample( int nr )
{
	if(!nonCarsLoaded)
	{
		char* path = Content::pathTo("./Content/TrainImages/neg-%d.pgm", nr);	
		return imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	}else{
		return nonCarSamples[nr-1];
	}	
}

cv::Mat Importer::loadCarImage( int nr )
{
	if(!carsLoaded)
	{
		char* path = Content::pathTo("./Content/TrainImages/pos-%d.pgm", nr);	
		return imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	}else{
		return carSamples[nr-1];
	}	
}

cv::Mat Importer::loadTestImage( int nr )
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

char* Importer::videoPath( int nr )
{
	return Content::pathTo("./Content/Videos/video-%d.avi", nr);
}





