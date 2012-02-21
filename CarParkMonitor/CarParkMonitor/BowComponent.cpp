#include "BowComponent.h"
#include <assert.h>
#include "Helper.h"

BowComponent::BowComponent( Importer* importer )
{
	assert(importer != NULL);

	this->importer = importer;

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
	vector<Mat> carSamples = importer->loadCarImages();					
	Mat allDescriptors = computeKeypointsAndDescriptors(carSamples, &carKeypoints, &carDescriptors);
	return allDescriptors;
}

cv::Mat BowComponent::getNonCarDescriptors()
{		
	vector<Mat> nonCarSamples = importer->loadNonCarImages();		
	Mat allDescriptors = computeKeypointsAndDescriptors(nonCarSamples, &nonCarKeypoints, &nonCarDescriptors);
	return allDescriptors;
}

cv::Mat BowComponent::computeCarBows()
{
	vector<Mat> images = importer->loadCarImages();
	Mat bows = computeBows(images, carKeypoints);
	return bows;
}

cv::Mat BowComponent::computerNonCarBows()
{
	vector<Mat> images = importer->loadNonCarImages();
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
	bowDE->compute(image, keypoints, bag);
	
	return bag;	
}

cv::Mat BowComponent::getVocabulary()
{
	return this->vocabulary;
}

vector<Mat> BowComponent::getImagesDescriptors()
{
	return this->carDescriptors;
}

vector<vector<KeyPoint>> BowComponent::getImagesKeypoints()
{
	return carKeypoints;		
}

#pragma region storage

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

	Helper::writeKeypointCollectionVec(f, "psize", "pks", carKeypoints);
	Helper::writeKeypointCollectionVec(f, "nsize", "nks", nonCarKeypoints);
	Helper::writeMatVec(f, "cdesc", carDescriptors);

	f.release();
}

void BowComponent::loadKeypointsAndDescriptors()
{
	FileStorage f = FileStorage(Content::ymlFile("kpds"), FileStorage::READ);

	f["positiveDescriptors"] >> positiveDescriptors;
	f["negativeDescriptors"] >> negativeDescriptors;

	Helper::readKeypointCollectionVec(f,"psize", "pks", &carKeypoints);
	Helper::readKeypointCollectionVec(f, "nsize", "nks", &nonCarKeypoints);
	Helper::readMatVec(f, "cdesc", &carDescriptors);

	f.release();

	assert(positiveDescriptors.rows > 0);
	assert(negativeDescriptors.rows > 0);
	assert(carKeypoints.size() > 0);
	assert(nonCarKeypoints.size() > 0);
}

void BowComponent::drawKeypoints( int imageIndex )
{
	vector<Mat> allImages = importer->loadCarImages();
	Mat image = allImages[imageIndex];
	vector<KeyPoint> keypoints = carKeypoints[imageIndex];

	Mat img_keypoint;
	cv::drawKeypoints( image, keypoints, img_keypoint, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	imshow("bow test", img_keypoint);
}

void BowComponent::loadLastConfig()
{
	loadVocabulary();
	loadKeypointsAndDescriptors();
	loadBows();	
}

#pragma endregion storage