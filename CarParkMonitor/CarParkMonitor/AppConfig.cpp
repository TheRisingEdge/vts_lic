#include "AppConfig.h"

int AppConfig::carSampleCount;
int AppConfig::nonCarSampleCount;
int AppConfig::clusterCount;
int AppConfig::partitionTestIndex;
int AppConfig::partitionCount;
int AppConfig::partitionSize;

BowProperties AppConfig::bowProperties;
CvSVMParams AppConfig::svmParams;

AppConfig::AppConfig(void)
{

}

AppConfig::~AppConfig(void)
{

}

FeatureDetector* configureFeatureDetector()
{
	//return new cv::GoodFeaturesToTrackDetector(
	//	50, // maximum number of corners to be returned
	//	0.1, // quality level
	//	5); // minimum allowed distance between points

	 return new cv::SiftFeatureDetector(0.1,20);	
}

DescriptorExtractor* configureDescriptorExtractor()
{
	return new SiftDescriptorExtractor();
};

DescriptorMatcher* configureDescriptorMatcher()
{
	return new BruteForceMatcher<L2<float>>();
}

void AppConfig::load()
{
	AppConfig::carSampleCount		= 550;
	AppConfig::nonCarSampleCount	= 450;	
	AppConfig::clusterCount			= 900;

	AppConfig::svmParams.svm_type    = SVM::C_SVC;
	AppConfig::svmParams.C           = 80;
	AppConfig::svmParams.kernel_type = SVM::RBF;
	AppConfig::svmParams.term_crit   = TermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);

	AppConfig::partitionCount = 5;
	AppConfig::partitionTestIndex = 4;
	AppConfig::partitionSize = AppConfig::carSampleCount/AppConfig::partitionCount;


	AppConfig::bowProperties.detector  = configureFeatureDetector();
	AppConfig::bowProperties.extractor = configureDescriptorExtractor();
	AppConfig::bowProperties.matcher   = configureDescriptorMatcher();
}

void AppConfig::loadFromYml()
{

}

