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

void AppConfig::load()
{
	AppConfig::carSampleCount = 500;
	AppConfig::nonCarSampleCount = 400;	
	AppConfig::clusterCount = 1000;

	AppConfig::svmParams.svm_type    = SVM::C_SVC;
	AppConfig::svmParams.C           = 50;
	AppConfig::svmParams.kernel_type = SVM::RBF;
	AppConfig::svmParams.term_crit   = TermCriteria(CV_TERMCRIT_ITER, 10000, 1e-6);


	AppConfig::bowProperties.detector = new SiftFeatureDetector();
	AppConfig::bowProperties.extractor = new SiftDescriptorExtractor();
	AppConfig::bowProperties.matcher = new BruteForceMatcher<L2<float>>();
}

void AppConfig::loadFromYml()
{

}


