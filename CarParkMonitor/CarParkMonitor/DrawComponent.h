#pragma once
#include "PlsaComponent.h"
#include "BowComponent.h"
class DrawComponent
{
public:

	PlsaComponent* plsaComponent;
	BowComponent* bowComponent;
	Importer* importer;

	DrawComponent(BowComponent* bow, PlsaComponent* plsa);	
	~DrawComponent(void);

	void drawPlsa(Mat image, Scalar* topicColors = NULL);
	void drawPlsa(vector<Mat> images);
	void drawKeypoints(char* windowName, Mat image);
	void drawKeypointsOnAllImages();
};

