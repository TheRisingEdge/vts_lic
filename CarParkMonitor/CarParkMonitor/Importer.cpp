#include "Importer.h"

Importer::~Importer(void)
{
}

Importer::Importer( void )
{
	nonCarsLoaded = false;
	allCarsLoaded = false;
	testCarsLoaded = false;
}

#pragma region helpers

void loadImagesWithIndexPath(char* indexPath, int count, vector<Mat>* output)
{
	for(int i = 1; i < count; i++)
	{
		char* path = Content::pathTo(indexPath, i);	
		Mat image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
		output->push_back(image);
	}
}

#pragma  endregion helpers

vector<Mat> Importer::readCarImages()
{
	if(!allCarsLoaded)
	{
		int carCount	=  AppConfig::carSampleCount;	
		loadImagesWithIndexPath("./Content/TrainImages/pos-%d.pgm", carCount, &allCars);		

		allCarsLoaded = true;
		testCarsLoaded = true;

		int size = allCars.size();
		for(int i = 0; i < size; i++)
		{
			bool isIncluded = ((i/AppConfig::partitionSize) != AppConfig::partitionTestIndex);
			Mat image = allCars[i];
			if(isIncluded){
				trainingCars.push_back(image);
			}else{
				testCars.push_back(image);
			}
		}
	}
	
	return trainingCars;
}

vector<Mat> Importer::loadNonCarImages()
{
	if(!nonCarsLoaded)
	{
		int nonCarCount	=  AppConfig::nonCarSampleCount;
		loadImagesWithIndexPath("./Content/TrainImages/neg-%d.pgm", nonCarCount, &allNonCars);	
		nonCarsLoaded = true;
	}

	return allNonCars;
}

void  Importer::loadAllImages()
{						
	readCarImages();	
	loadNonCarImages();
}

cv::Mat Importer::loadNonCarSample( int nr )
{
	if(!nonCarsLoaded)
	{
		char* path = Content::pathTo("./Content/TrainImages/neg-%d.pgm", nr);	
		return imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	}else{
		return allNonCars[nr-1];
	}	
}

cv::Mat Importer::loadCarImage( int nr )
{
	if(!allCarsLoaded)
	{
		char* path = Content::pathTo("./Content/TrainImages/pos-%d.pgm", nr);	
		return imread(path, CV_LOAD_IMAGE_GRAYSCALE);

	}else{
		return allCars[nr-1];
	}	
}

vector<Mat> Importer::loadTestImages()
{
	if(!testCarsLoaded)
		readCarImages();

	return testCars;
}

cv::Mat Importer::loadTestImage( int nr )
{
	char* path = Content::pathTo("./Content/TestImages/test-%d.pgm",nr);
	return imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

#pragma region notused

cv::Mat Importer::loadGrayImage( char* fileName )
{
	char* path = new char[100];
	sprintf(path, "./Content/Images/%s", fileName);
	return imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

vector<Mat> Importer::loadCarImages()
{
	if(!allCarsLoaded)
		readCarImages();

	return trainingCars;
}

#pragma endregion notused









