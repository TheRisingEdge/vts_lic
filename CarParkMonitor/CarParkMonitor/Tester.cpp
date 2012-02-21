#include "Tester.h"
#include "Helper.h"
#include <conio.h>

typedef struct testResult
{
	char* message;
	int totalTested;
	int missclassified;
	float errorRate;

	void computeErrorRate()
	{
		this->errorRate = (float)(missclassified*100)/(float)totalTested;
	}

}TestResult;

void printToConsole(TestResult result)
{
	cout << result.message << "\n";
	cout << "total tested: " << result.totalTested << "\n";
	cout << "missclassified: " << result.missclassified << "\n";
	cout << "error rate: " << result.errorRate << "\n";
}

void printToFile(TestResult result)
{
	FileStorage f = FileStorage(Content::ymlFile("testResults"), FileStorage::APPEND);
	
	f << "message" << result.message;
	f << "total tested" << result.totalTested;
	f << "missclassified" << result.missclassified;
	f << "error rate" << result.errorRate;

	f.release();
}


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



void Tester::testPositives(vector<Mat>* images)
{
	vector<Mat> imgs;
	if(images == NULL)
	{
		imgs = importer->loadCarImages();
		images = &imgs;	
	}
	
	int size = images->size();
	int wrongs = 0;

	for(int i = 0; i < size; i++)
	{
		Mat image = images->at(i);
		Mat bow = bowComponent->extractBow(image);
		float prediction = svm->predict(bow);

		if(prediction < 0)
			wrongs++;
	}

	TestResult r;
	r.totalTested = size;
	r.missclassified = wrongs;
	r.computeErrorRate();
	r.message = "positive images test";

	printToConsole(r);
	printToFile(r);

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
		
		if(bow.cols == 0 || bow.rows == 0)
			continue;

		float prediction = svm->predict(bow);

		if(prediction > 0)
			wrongs++;
	}

	TestResult r;
	r.totalTested = size;
	r.missclassified = wrongs;
	r.computeErrorRate();
	r.message = "negative images test";

	printToConsole(r);
	printToFile(r);

	getch();
}

void Tester::test()
{
	
	vector<Mat> images = importer->loadTestImages();	
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

	TestResult r;
	r.totalTested = size;
	r.missclassified = wrongs;
	r.computeErrorRate();
	r.message = "positive images test";

	printToConsole(r);
	printToFile(r);

	getch();
}

void Tester::testFiltered( int* filteredColumns, int count )
{
	vector<Mat> images;
	images = importer->loadTestImages();


	int size = images.size();
	int wrongs = 0;

	for(int i = 0; i < size; i++)
	{
		Mat image = images.at(i);
		Mat bow = bowComponent->extractBow(image);
		Mat bownew = Helper::filterColumns(bow, filteredColumns, count);
		float prediction = svm->predict(bownew);

		if(prediction < 0)
			wrongs++;
	}

	TestResult r;
	r.totalTested = size;
	r.missclassified = wrongs;
	r.computeErrorRate();
	r.message = "positive images test, plsa on all cars";

	printToConsole(r);
	printToFile(r);

	getch();
}


void Tester::testFilteredNegatives( int* filteredColumns, int count )
{
	vector<Mat> images;
	images = importer->loadNonCarImages();


	int size = images.size();
	int wrongs = 0;

	for(int i = 0; i < size; i++)
	{
		Mat image = images.at(i);
		Mat bow = bowComponent->extractBow(image);
		if(bow.rows == 0)
			continue;

		Mat bownew = Helper::filterColumns(bow, filteredColumns, count);
		float prediction = svm->predict(bownew);

		if(prediction > 0)
			wrongs++;
	}

	TestResult r;
	r.totalTested = size;
	r.missclassified = wrongs;
	r.computeErrorRate();
	r.message = "negative images test";

	printToConsole(r);
	printToFile(r);

	getch();
}

void Tester::testPositiveImage( Mat image )
{

}
