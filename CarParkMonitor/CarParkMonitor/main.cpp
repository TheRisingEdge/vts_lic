#include <stdio.h>
#include "Importer.h"
#include "BowComponent.h"
#include "SvmComponent.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include "Tester.h"
#include "PlsaComponent.h"

using namespace std;

int main(int argc, char** argv)
{
	//AppConfig::load();	
	//Importer importer;

	//bool training = true;
	//SvmComponent svm;
	//BowComponent bowComponent(&importer);	
	//
	//if(training)
	//{		
	//	importer.loadTrainingImages();		
	//	bowComponent.extractBows(true, true, true);		
	//	svm.train(bowComponent.positiveBows, bowComponent.negativeBows);
	//	svm.save();

	//}else{
	//	svm.load();			

	//	Tester tester(&bowComponent, &svm);		
	//	tester.test();		
	//}

	float data[5][6] = {
		{9.0, 2.0, 1.0, 1.0, 1.0, 0.0},   
		{8.0, 3.0, 2.0, 1.0, 0.0, 0.0},   
		{3.0, 0.0, 0.0, 1.0, 2.0, 8.0},
		{0.0, 1.0, 0.0, 2.0, 4.0, 7.0},
		{2.0, 1.0, 1.0, 0.0, 1.0, 3.0},   
	};

	float** bows = new float*[5];
	for(int i = 0; i < 5; i++)
	{
		bows[i] = new float[6];
		for(int j = 0; j < 6; j++)
		{
			bows[i][j] = data[i][j];
		}
	}

	PlsaComponent* plsa = new PlsaComponent(bows, 5, 6, 3);
	plsa->performPLSA();

	getch();
	return 0;
}