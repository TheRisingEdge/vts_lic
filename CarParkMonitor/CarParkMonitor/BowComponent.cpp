#include "BowComponent.h"
#include <assert.h>



BowComponent::BowComponent(void)
{						
	this->detector		= Ptr<FeatureDetector>(AppConfig::bowProperties.detector); 			
	this->extractor		= Ptr<DescriptorExtractor>(AppConfig::bowProperties.extractor);
	this->matcher		= Ptr<DescriptorMatcher>(AppConfig::bowProperties.matcher);

	this->bowDE			= new BOWImgDescriptorExtractor(extractor,matcher);
	this->bowTrainer	= new BOWKMeansTrainer(AppConfig::clusterCount);	

	vocabularyLoaded = false;
}

BowComponent::~BowComponent(void)
{

}

void BowComponent::extractBows(bool extractDescriptors, bool extractVocabulary , bool extractBows )
{
	if(extractDescriptors)
	{
		positiveDescriptors = getCarDescriptors();
		negativeDescriptors	= getNonCarDescriptors();
		saveKeypointsAndDescriptors();
		
	}else{
		loadKeypointsAndDescriptors();
	}

	if(extractVocabulary)
	{
		bowTrainer->add(positiveDescriptors);
		
		vocabulary = bowTrainer->cluster();
		bowDE->setVocabulary(vocabulary);

		saveVocabulary();
	}else{
		loadVocabulary();	
	}
	
	if(extractBows)
	{
		positiveBows	= computeCarBows();
		negativeBows	= computerNonCarBows();
		saveBows();
	}else{
		loadBows();
	}
}

cv::Mat BowComponent::computeBows( const vector<Mat> images,const vector<vector<KeyPoint>> keypoints )
{
	Mat bags;

	int count = images.size();
	for(int i = 0; i < count; i++){			
		vector<KeyPoint> kps = keypoints[i];
		if(kps.size() != 0){
			Mat img = images[i];
			Mat vocBag;
			bowDE->compute(img, kps, vocBag);
			bags.push_back(vocBag);
		}
	}

	return bags;
}

cv::Mat BowComponent::computeKeypointsAndDescriptors( vector<Mat> images,vector<vector<KeyPoint>>* keypoints_out, vector<Mat>* descriptors_out )
{				
	Mat allDescriptors;

	detector->detect(images, *keypoints_out);
	extractor->compute(images, *keypoints_out, *descriptors_out);

	int count = (*descriptors_out).size();
	for(int i = 0; i < count; i++){
		Mat desc = (*descriptors_out)[i];
		if(desc.rows != 0){
			allDescriptors.push_back(desc);
		}
	}

	return allDescriptors;
}

cv::Mat BowComponent::getCarDescriptors()
{			
	vector<Mat> carSamples = Importer::getCarSamples();					
	Mat allDescriptors = computeKeypointsAndDescriptors(carSamples, &carKeypoints, &carDescriptors);
	return allDescriptors;
}

cv::Mat BowComponent::getNonCarDescriptors()
{		
	vector<Mat> nonCarSamples = Importer::getNonCarSamples();		
	Mat allDescriptors = computeKeypointsAndDescriptors(nonCarSamples, &nonCarKeypoints, &nonCarDescriptors);
	return allDescriptors;
}

cv::Mat BowComponent::computeCarBows()
{
	vector<Mat> images = Importer::getCarSamples();
	Mat bows = computeBows(images, carKeypoints);
	return bows;
}

cv::Mat BowComponent::computerNonCarBows()
{
	vector<Mat> images = Importer::getNonCarSamples();
	Mat bows = computeBows(images, nonCarKeypoints);
	return bows;
}

Mat_<float> BowComponent::extractBow( Mat image )
{
	if(!vocabularyLoaded){
		loadVocabulary();
	}

	vector<KeyPoint> keypoints;
	Mat_<float> descriptors;
	Mat_<float> bag;

	detector->detect(image, keypoints);
	assert(keypoints.size() > 0);

	bowDE->compute(image, keypoints, bag);
	
	return bag;	
}

void BowComponent::saveVocabulary()
{
	FileStorage f = FileStorage(Content::ymlFile("bowConfig"), FileStorage::WRITE);
	f << "vocabulary" << vocabulary;
	f.release();
}

void BowComponent::loadVocabulary()
{
	assert(bowDE != NULL);

	FileStorage f = FileStorage(Content::ymlFile("bowConfig"), FileStorage::READ);
	f["vocabulary"] >> vocabulary;
	f.release();

	vocabularyLoaded = true;
	bowDE->setVocabulary(vocabulary);
	
	assert(vocabulary.rows > 0);
	assert(vocabulary.cols > 0);
}

void BowComponent::saveKeypointsAndDescriptors()
{
	FileStorage f = FileStorage(Content::ymlFile("kpds"), FileStorage::WRITE);
	f << "positiveDescriptors" << positiveDescriptors;
	f << "negativeDescriptors" << negativeDescriptors;
	writeKeypointsVec(f, "psize", "pks", carKeypoints);
	writeKeypointsVec(f, "nsize", "nks", nonCarKeypoints);
	f.release();
}

void BowComponent::loadKeypointsAndDescriptors()
{
	FileStorage f = FileStorage(Content::ymlFile("kpds"), FileStorage::READ);
	
	f["positiveDescriptors"] >> positiveDescriptors;
	f["negativeDescriptors"] >> negativeDescriptors;
	readKeypointsVec(f, "psize", "pks", &carKeypoints);
	readKeypointsVec(f, "nsize", "nks", &nonCarKeypoints);
	
	f.release();

	assert(positiveDescriptors.rows > 0);
	assert(negativeDescriptors.rows > 0);
	assert(carKeypoints.size() > 0);
	assert(nonCarKeypoints.size() > 0);
}

void BowComponent::writeKeypointsVec( FileStorage f, const char* sizeKey,const char* baseKey, vector<vector<KeyPoint>>  keypointsVec)
{
	int size = keypointsVec.size();
	
	f << sizeKey << size;

	for(int i = 0; i < size;i++)
	{
		vector<KeyPoint> keypoints = keypointsVec[i];
		
		char* key = new char[50];
		sprintf(key, "%s%d", baseKey,i);
		
		write(f, key, keypoints);	
	}	
}

void BowComponent::readKeypointsVec( FileStorage f,const char* sizeKey, const char* keyName, vector<vector<KeyPoint>>* keypointsVec )
{
	int size = 0;
	f[sizeKey] >> size;

	vector<KeyPoint> keypoints;
	for(int i = 0; i < size;i++)
	{		
		char* key = new char[20];
		sprintf(key, "%s%d", keyName, i);
				
		read(f[key], keypoints);
		keypointsVec->push_back(keypoints);
	}	

	assert(keypointsVec->size() > 0);
}

void BowComponent::saveBows()
{
	FileStorage f = FileStorage(Content::ymlFile("bows"), FileStorage::WRITE);
	f << "positiveBows" << positiveBows;
	f << "negativeBows" << negativeBows;
	f.release();
}

void BowComponent::loadBows()
{
	FileStorage f = FileStorage(Content::ymlFile("bows"), FileStorage::READ);
	f["positiveBows"] >> positiveBows;
	f["negativeBows"] >> negativeBows;
	f.release();
}

void BowComponent::drawKeypoints( Mat image )
{
	if(!vocabularyLoaded)
	{
		loadVocabulary();		
	}

	vector<KeyPoint> keypoints;
	detector->detect(image, keypoints);	

	cv::drawKeypoints(image, // original image
		keypoints, // vector of keypoints
		image, // the resulting image
		cv::Scalar(255,0,0), // color of the points
		cv::DrawMatchesFlags::DEFAULT //flag	
	); 

	imshow("keypoints", image);
}

