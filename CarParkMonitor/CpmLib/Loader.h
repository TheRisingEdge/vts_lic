#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Loader
{
private:
	static char* positivesPath;
	static char* negativesPath;

	static int positivesCount;
	static int negativesCount;

	static bool positivesLoaded;
	static bool negativesLoaded;

	static vector<Mat> positiveSamples;
	static vector<Mat> negativeSamples;
	static void loadPositiveSamples();
	static void loadNegativeSamples();
	
public:
	Loader(void){}
	~Loader(void)
	{
		delete[] positivesPath;
		delete[] negativesPath;
	}

	static vector<Mat> getPositiveSamples();
	static vector<Mat> getNegativeSamples();

	static void setPosPathAndCount(char* path, int posCount)
	{
		positivesPath = path;
		positivesCount = posCount;
	}
	static void setNegPathAndCount(char* path, int negCount)
	{
		negativesPath = path;
		negativesCount = negCount;
	}
};

