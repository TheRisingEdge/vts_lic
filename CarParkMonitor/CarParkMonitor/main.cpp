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
		Importer::loadSamples();
		vector<Mat> samples = Importer::getCarSamples();
		int count = 0;
		for(int i = 1; i < samples.size(); i++)
		{
			//Mat image = Importer::loadNonCarSample(i);			

			//char* windowName = new char[10];
			//sprintf(windowName, "%d", i);

			Mat image =  samples[i];
			//bowComponent.drawKeypoints(image, windowName);
			Mat bow = bowComponent.extractBow(image);
			float prediction = svm.predict(bow);
			cout << prediction << "\n";

			if(prediction < 0){
				//imshow(Content::pathTo("%d", i), image);
				count++;
			}
		}

		cout << count;
		/*Mat image = Importer::loadGrayImage(Content::file("test01.pgm"));
		Mat bow = bowComponent.extractBow(image);
		float prediction = svm.predict(bow);*/
	
	}
	getch();
	waitKey();
	return 0;
}