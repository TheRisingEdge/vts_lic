#pragma once
#include "AppConfig.h"
#include "Importer.h"

using namespace cv;

class BowComponent
{
private:	
	Ptr<DescriptorMatcher>		matcher;
	BOWImgDescriptorExtractor*	bowDE; 
	BOWKMeansTrainer*			bowTrainer;

	vector<vector<KeyPoint>> carKeypoints;
	vector<vector<KeyPoint>> nonCarKeypoints;

	vector<Mat> carDescriptors;
	vector<Mat> nonCarDescriptors;

	Mat_<float> positiveDescriptors;
	Mat_<float> negativeDescriptors;

	void saveBows();
	
	void saveKeypointsAndDescriptors();
	void loadKeypointsAndDescriptors();

	void writeDescriptorsVec( FileStorage f, const char*  sizeKey, const char* baseKey, vector<Mat> carDescriptors );
	void readDescriptorsVec( FileStorage f, const char*  sizeKey, const char* baseKey, vector<Mat>* carDescriptors );
	void writeKeypointsVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>> carKeypoints );
	void readKeypointsVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>>* carKeypoints );

	Mat computeKeypointsAndDescriptors(vector<Mat> images,vector<vector<KeyPoint>>* keypoints_out, vector<Mat>* descriptors_out);
	Mat getCarDescriptors();
	Mat getNonCarDescriptors();
	Mat computeBows(const vector<Mat> images,const vector<vector<KeyPoint>> keypoints );
	Mat computeCarBows();
	Mat computerNonCarBows();
	vector<Mat> getImagesDescriptors();
	vector<vector<KeyPoint>> getImagesKeypoints();	
	void saveVocabulary();
	void loadVocabulary();
	void loadBows();

public:
	Ptr<FeatureDetector>		detector;
	Ptr<DescriptorExtractor>	extractor;

	Importer* importer;
	BowComponent(Importer* importer);	
	~BowComponent(void);
	
	Mat vocabulary;
	Mat positiveBows;
	Mat negativeBows;
	bool vocabularyLoaded;

	void extractBows( bool extractDescriptors = true, bool extractVocabulary = true, bool extractBows = true);			
	Mat_<float> extractBow(Mat image);

	void loadLastConfig();
	Mat getVocabulary();

	void drawKeypoints(int imageIndex);	
};

