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

	void testPositives(vector<Mat>* images = NULL);
	void testNegatives();
};

