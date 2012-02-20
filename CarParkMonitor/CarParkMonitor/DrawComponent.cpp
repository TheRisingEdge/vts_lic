#include "DrawComponent.h"
#include "Helper.h"
#include "opencv2\features2d\features2d.hpp"
#include <algorithm>

using namespace cv;
using namespace std;
#pragma  region constructors

DrawComponent::DrawComponent( BowComponent* bow, PlsaComponent* plsa )
{
	this->bowComponent = bow;
	this->plsaComponent = plsa;
	this->importer = bow->importer;
}


DrawComponent::~DrawComponent(void)
{
}

#pragma endregion constructors

#pragma region helpers

Scalar* getRandomColors(int length)
{
	Scalar* colors = new Scalar[length];

	srand(time(NULL));
	for(int i = 0; i < length; i++)
	{
		int r = rand()%255;
		int g = rand()%255;
		int b = rand()%255;

		colors[i] = Scalar(r,g,b);
	}

	return colors;
}

int* getWordTopics(float** topicWordMat, int rows, int cols)
{
	int* maxProbTopic = new int[cols];

	for(int i = 0; i < cols; i++)
	{
		float max = 0;
		int topic = 0;
		for(int j = 0; j < rows; j++)
		{
			if(topicWordMat[j][i] > max)
			{
				max = topicWordMat[j][i];
				topic = j;
			}
		}

		maxProbTopic[i] = topic;
	}

	return maxProbTopic;
}

#pragma endregion helpers

void DrawComponent::drawPlsa(Mat image, Scalar* topicColors)
{
	assert(plsaComponent != NULL);
	assert(bowComponent != NULL);			
	
	float** wordProbabilities = plsaComponent->getTopicWordMatrix();

	int* wordTopic = getWordTopics(
		plsaComponent->getTopicWordMatrix(),
		plsaComponent->numberOfTopics,
		plsaComponent->numberOfColumns
	);

	if(topicColors == NULL)
		topicColors = getRandomColors(plsaComponent->numberOfTopics);

	vector<KeyPoint> keypoints;
	Mat descriptors;

	bowComponent->detector->detect(image, keypoints);
	bowComponent->extractor->compute(image, keypoints, descriptors);

	cv::BruteForceMatcher<cv::L2<float>> matcher;
	
	Mat vocabulary  = bowComponent->getVocabulary();
	vector<vector<cv::DMatch>> matches;
	
	Mat imageOut;
	cv::cvtColor(image, imageOut, CV_GRAY2BGR);

	matcher.knnMatch(descriptors, vocabulary, matches, 1);	
	for(int i = 0; i < matches.size(); i++)
	{
		vector<DMatch> pointMatches = matches[i];
		DMatch match = pointMatches[0];
		int vocabularyInd = match.trainIdx;
		int topic = wordTopic[vocabularyInd];

		Point p = keypoints[i].pt;
		Scalar color = topicColors[topic];

		cv::circle(imageOut, p, 3, color);
	}

	imshow("topics", imageOut);
}

void DrawComponent::drawPlsa( vector<Mat> images )
{
	int size = images.size();
	Scalar* topicColors = getRandomColors(plsaComponent->numberOfTopics);

	for(int i = 0 ; i < size; i++)
	{
		Mat image = images[i];
		this->drawPlsa(image, topicColors);
		waitKey();
	}
}

void DrawComponent::drawKeypoints( char* windowName ,Mat image )
{
	assert(bowComponent != NULL);
	assert(bowComponent->detector != NULL);

	vector<KeyPoint> keypoints;		
	bowComponent->detector->detect(image, keypoints);

	Mat img_keypoint;
	cv::drawKeypoints( image, keypoints, img_keypoint, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	imshow(windowName, img_keypoint);
}

void DrawComponent::drawKeypointsOnAllImages()
{
	vector<Mat> images = importer->loadCarImages();
	
	for(int i = 0 ; i < images.size(); i++)
	{
		this->drawKeypoints("all keypoints", images[i]);
		waitKey();
	}	
}

