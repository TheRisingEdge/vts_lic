#include <stdio.h>
#include "Importer.h"
#include "BowComponent.h"
#include "SvmComponent.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include "Tester.h"

using namespace std;

int main(int argc, char** argv)
{
	AppConfig::load();	
	Importer importer;

	bool training = false;
	SvmComponent svm;
	BowComponent bowComponent(&importer);	
	
	if(training)
	{		
		importer.loadTrainingImages();		
		bowComponent.extractBows(false, false, false);		
		svm.train(bowComponent.positiveBows, bowComponent.negativeBows);
		svm.save();

	}else{
		svm.load();			
		vector<Mat>* imgs = new vector<Mat>(); 
		imgs->push_back(importer.loadGrayImage("test01.pgm"));

		Tester tester(&bowComponent, &svm);
		//tester.testPositives(NULL);	
		tester.testNegatives();
	}
	waitKey();
	return 0;
}