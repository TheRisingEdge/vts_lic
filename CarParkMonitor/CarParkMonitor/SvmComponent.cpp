#include "SvmComponent.h"
#include "AppConfig.h"
#include <iostream>
#include <assert.h>

using namespace std;

SvmComponent::SvmComponent(void)
{
}

SvmComponent::~SvmComponent(void)
{
}

void SvmComponent::train( Mat_<float> positiveSamples, Mat_<float> negativeSamples )
{	
	assert(positiveSamples.rows > 0);
	assert(negativeSamples.cols > 0);

	Mat positiveLabels(positiveSamples.rows, 1, CV_32FC1, Scalar(1.0));
	Mat negativeLabels(negativeSamples.rows, 1, CV_32FC1, Scalar(-1.0));
	
	Mat_<float> trainingData;
	trainingData.push_back(positiveSamples);
	trainingData.push_back(negativeSamples);	

	Mat_<float> trainingLabels;
	trainingLabels.push_back(positiveLabels);
	trainingLabels.push_back(negativeLabels);

	cout << "SVM training started \n";
	cout << "\t positive features: " << positiveSamples.rows << "\n";
	cout << "\t negative features: " << negativeSamples.rows << "\n";

	this->svm.train(trainingData, trainingLabels, Mat(), Mat(), AppConfig::svmParams);	
	this->save();
	cout << "SVM training ended (saved SVM) \n";
	cout << "\t support vectors: " << svm.get_support_vector_count() << "\n";
}

void SvmComponent::save()
{
	char* path = Content::ymlFile("svm");
	svm.save(path);
}

void SvmComponent::load()
{
	char* path = Content::ymlFile("svm");
	svm.load(path);
	cout << "SVM loaded, support vectors: " << svm.get_support_vector_count() << "\n"; 
}

float SvmComponent::predict( Mat sample )
{
	float prediction = svm.predict(sample);
	return prediction;
}



