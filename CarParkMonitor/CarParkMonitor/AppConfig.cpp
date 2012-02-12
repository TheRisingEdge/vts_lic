#include "AppConfig.h"

int AppConfig::carSampleCount;
int AppConfig::nonCarSampleCount;
int AppConfig::clusterCount;

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
	//return new GoodFeaturesToTrackDetector(50, 0.1, 0.5);
	//return new GoodFeaturesToTrackDetector(100,0.1,0.2,3, true, 0.004);
	//void goodFeaturesToTrack(InputArray image, OutputArray corners, int maxCorners, double qualityLevel, double minDistance, InputArray mask=noArray(), int blockSize=3, bool useHarrisDetector=false, double k=0.04 )¶
	return new cv::GoodFeaturesToTrackDetector(
		50, // maximum number of corners to be returned
		0.1, // quality level
		5); // minimum allowed distance between points
	
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
	AppConfig::clusterCount			= 600;

	AppConfig::svmParams.svm_type    = SVM::C_SVC;
	AppConfig::svmParams.C           = 80;
	AppConfig::svmParams.kernel_type = SVM::RBF;
	AppConfig::svmParams.term_crit   = TermCriteria(CV_TERMCRIT_ITER, 100000, 1e-6);


	AppConfig::bowProperties.detector  = configureFeatureDetector();
	AppConfig::bowProperties.extractor = configureDescriptorExtractor();
	AppConfig::bowProperties.matcher   = configureDescriptorMatcher();
}

void AppConfig::loadFromYml()
{

}


