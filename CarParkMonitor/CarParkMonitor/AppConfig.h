#pragma once
#include "Content.h"
#include "opencv2/opencv.hpp"

using namespace cv;

typedef struct bowProperties
{
	FeatureDetector* detector;
	DescriptorExtractor* extractor;
	DescriptorMatcher* matcher;

}BowProperties;

typedef struct databaseProperties
{
	int carSampleCount;
	int nonCarSampleCount;
	char* rootFolder;

}DatabaseProperties;

class AppConfig
{
public:
	AppConfig(void);
	~AppConfig(void);

	/* Samples */	
	static int carSampleCount;
	static int nonCarSampleCount;

	/* BOW Params */		
	static int clusterCount;

	/* SVM Params */
	static CvSVMParams svmParams;

	/*Bow Config*/
	static BowProperties bowProperties;

	static DatabaseProperties databaseProperties;

	static void load();
	static void loadFromYml();
};

