#include "Loader.h"

vector<Mat> Loader::positiveSamples;
vector<Mat> Loader::negativeSamples;

bool Loader::positivesLoaded = false;
bool Loader::negativesLoaded = false;

int Loader::positivesCount;
int Loader::negativesCount;

char* Loader::positivesPath;
char* Loader::negativesPath;
int Loader::baseNegative = 0;

void Loader::loadPositiveSamples()
{
	if(positivesLoaded)
		return;

	int size = positivesCount;
	positiveSamples.reserve(size);

	char* path = new char[100];

	for(int i = 1; i < size; i++)
	{		
		sprintf(path, "%s/pos-%d.png",positivesPath, i);	
		Mat image = imread(path);
		assert(image.rows > 0 && image.cols > 0);
		positiveSamples.push_back(image);
	}
		
	positivesLoaded = true;
	delete[] path;
}

void Loader::loadNegativeSamples()
{
	if(negativesLoaded)
		return;

	int size = negativesCount;
	negativeSamples.reserve(size);

	char* path = new char[100];

	for(int i = baseNegative; i < baseNegative+size; i++)
	{
		sprintf(path, "%s/neg-%d.png", negativesPath, i);
		Mat image = imread(path);
		assert(image.rows > 0 && image.cols > 0);
		negativeSamples.push_back(image);
	}

	negativesLoaded = true;
	delete[] path;
}


vector<Mat> Loader::getPositiveSamples()
{
	if(!positivesLoaded)
		loadPositiveSamples();

	return positiveSamples;
}

vector<Mat> Loader::getNegativeSamples()
{
	if(!negativesLoaded)
		loadNegativeSamples();

	return negativeSamples;
}


