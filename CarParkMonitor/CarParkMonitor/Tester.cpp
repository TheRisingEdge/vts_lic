#include "Tester.h"
#include <conio.h>

Tester::Tester(BowComponent* bowComponent, SvmComponent* svm)
{
	assert(svm != NULL);
	assert(bowComponent != NULL);
	assert(bowComponent->importer != NULL);

	this->svm = svm;
	this->bowComponent = bowComponent;
	this->importer = bowComponent->importer;
}


Tester::~Tester(void)
{
}

void Tester::testPositives()
{
	vector<Mat> images = importer->loadCarImages();
	int size = images.size();

	int wrongs = 0;

	for(int i = 0; i < size; i++)
	{
		Mat image = images[i];
		Mat bow = bowComponent->extractBow(image);
		float prediction = svm->predict(bow);

		if(prediction < 0)
			wrongs++;

	}

	cout << wrongs << " car images classified wrong" << "\n";
	float error = (float)(wrongs * 100)/(float)AppConfig::carSampleCount;
	cout << "error rate: " << error;
	getch();
}

void Tester::testNegatives()
{
	vector<Mat> images = importer->loadNonCarImages();
	int size = images.size();

	int wrongs = 0;

	for(int i = 0; i < size; i++)
	{
		Mat image = images[i];
		Mat bow = bowComponent->extractBow(image);
		float prediction = svm->predict(bow);

		if(prediction > 0)
			wrongs++;

	}

	cout << wrongs << " NON car images classified wrong" << "\n";
	float error = (float)(wrongs * 100)/(float)AppConfig::carSampleCount;
	cout << "error rate: " << error;
	getch();

}
