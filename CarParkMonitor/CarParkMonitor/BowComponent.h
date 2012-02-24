#pragma once
#include "AppConfig.h"
#include "Importer.h"

using namespace cv;

class BowComponent
{
private:	
	Mat vocabulary;
	bool vocabularyLoaded;

	Ptr<DescriptorMatcher>		matcher;
	BOWImgDescriptorExtractor*	bowDE; 
	BOWKMeansTrainer*			bowTrainer;

	vector<vector<KeyPoint>> carKeypoints;
	vector<vector<KeyPoint>> nonCarKeypoints;

	vector<Mat> carDescriptors;
	vector<Mat> nonCarDescriptors;

	Mat_<float> positiveDescriptors;
	Mat_<float> negativeDescriptors;
			
	Mat getCarDescriptors();
	Mat getNonCarDescriptors();
	Mat computeCarBows();
	Mat computerNonCarBows();

	void saveKeypointsAndDescriptors();
	void loadKeypointsAndDescriptors();

	void saveVocabulary();
	void loadVocabulary();

	void saveBows();
	void loadBows();
	
	Mat computeBows(const vector<Mat> images,const vector<vector<KeyPoint>> keypoints );
	Mat computeKeypointsAndDescriptors(vector<Mat> images,vector<vector<KeyPoint>>* keypoints_out, vector<Mat>* descriptors_out);

public:	
	Mat positiveBows;
	Mat negativeBows;
	
	Importer* importer;
	Ptr<FeatureDetector>	detector;
	Ptr<DescriptorExtractor> extractor;

	BowComponent(Importer* importer);	
	~BowComponent(void);
		
	void extractBows( bool extractDescriptors = true, bool extractVocabulary = true, bool extractBows = true);			
	Mat_<float> extractBow(Mat image);

	void loadLastConfig();
	Mat getVocabulary();

	void drawKeypoints(int imageIndex);	
};

