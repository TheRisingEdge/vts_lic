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
		bowComponent.extractBows(true, true, true);		
		svm.train(bowComponent.positiveBows, bowComponent.negativeBows);
		svm.save();

	}else{
		svm.load();			
		
		Tester tester(&bowComponent, &svm);
		//tester.testPositives();	
		tester.testNegatives();
	}
	getch();
	waitKey();
	return 0;
}