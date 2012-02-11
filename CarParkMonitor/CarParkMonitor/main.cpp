#include <stdio.h>
#include "Importer.h"
#include "BowComponent.h"
#include "SvmComponent.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>

using namespace std;

int main(int argc, char** argv)
{
	AppConfig::load();	

	bool training = false;
	SvmComponent svm;
	BowComponent bowComponent;	
	
	if(training)
	{		
		Importer::loadSamples();		
		bowComponent.extractBows(true, true, true);		
		svm.train(bowComponent.positiveBows, bowComponent.negativeBows);
		svm.save();

	}else{
		svm.load();			

		for(int i = 400; i < 450; i++)
		{
			Mat image = Importer::loadNonCarSample(i);			
			Mat bow = bowComponent.extractBow(image);
			float prediction = svm.predict(bow);
			cout << prediction << "\n";

			/*if(prediction < 0){
				imshow(Content::pathTo("%d", i), image);
			}*/
		}

		/*Mat image = Importer::loadGrayImage(Content::file("test01.pgm"));
		Mat bow = bowComponent.extractBow(image);
		float prediction = svm.predict(bow);*/
	
	}
	
	waitKey();
	return 0;
}