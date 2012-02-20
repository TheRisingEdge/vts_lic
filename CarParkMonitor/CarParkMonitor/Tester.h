#pragma once
#include "SvmComponent.h"
#include "Importer.h"
#include "BowComponent.h"

class Tester
{
private:
	BowComponent* bowComponent;
	SvmComponent* svm;
	Importer* importer;

public:	
	Tester(BowComponent* bowComponent, SvmComponent* svm);
	~Tester(void);

	void test();
	void testFiltered(int* filteredColumns, int count);
	void testPositives(vector<Mat>* images = NULL);
	void testNegatives();
	void testFilteredNegatives( int* filteredColumns, int count );
	void testPositiveImage(Mat image);
};

