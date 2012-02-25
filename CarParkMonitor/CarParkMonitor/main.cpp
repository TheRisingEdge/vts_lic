#include <stdio.h>
#include "Importer.h"
#include "BowComponent.h"
#include "SvmComponent.h"
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include "Tester.h"
#include "PlsaComponent.h"
#include "Helper.h"
#include "DrawComponent.h"
#include <process.h>
#include "VideoProcessor.h"
#include "BackgroundSegmentator.h"
#include "CannyFrameProcessor.h"
#include "FeatureTracker.h"
#include "BgFgSegmentator.h"

using namespace std;

int main(int argc, char** argv)
{

	char* video = Content::videoFile("camera1.avi");
	VideoProcessor videoProcessor(video);	
	FrameProcessor* segmentator = new BgFgSegmentator();
	videoProcessor.run(segmentator);






















	//AppConfig::load();	
	//Importer importer;


	//bool training = true;
	//SvmComponent svm;
	//BowComponent bowComponent(&importer);	
	//PlsaComponent plsa;
	//int* featureIndexes;
	//int keptFeatureCount = 500;
	//if(training)
	//{		
	//	importer.loadAllImages();				
	//	//bowComponent.extractBows(true, true, true);	
	//	bowComponent.loadLastConfig();
	//			
	//	//plsa.testPLSA();

	//	//DrawComponent draw = DrawComponent(&bowComponent, NULL);

	//	//draw.drawKeypointsOnAllImages();

	//	PlsaParam param = PlsaParam::FromCvMat(bowComponent.positiveBows, 1, 100000);
	//	param.keptFeatureCount = keptFeatureCount;		
	//	plsa.load();
	//	featureIndexes = plsa.relevantIndexes;//plsa.computeRelevantFeatures(param);
	//	//plsa.save();
	//	
	//	Mat_<float> newBows = Helper::filterColumns(bowComponent.positiveBows, featureIndexes, param.keptFeatureCount);
	//	Mat_<float> newNonBows = Helper::filterColumns(bowComponent.negativeBows, featureIndexes, param.keptFeatureCount);		
	//	Mat pos = bowComponent.positiveBows;//newBows;
	//	Mat neg = bowComponent.negativeBows;//newNonBows;

	//	svm.train(pos, neg);	
	//	//svm.load();

	//	Tester tester(&bowComponent, &svm);		
	//	//tester.testFiltered(featureIndexes, param.keptFeatureCount);
	//	tester.testNegatives();
	//	tester.testPositives();
	//	tester.testPositives(&(importer.loadTestImages()));

	//	//tester.testFilteredNegatives(featureIndexes, keptFeatureCount);

	//}else{
	//	svm.load();
	//	plsa.load();
	//	Tester tester(&bowComponent, &svm);		
	//	tester.testFiltered(featureIndexes, keptFeatureCount);
	//}

	//printf("press any key to continue");
	//getch();
	//waitKey();	
	return 0;
}