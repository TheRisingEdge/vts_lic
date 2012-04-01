#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Loader
{
private:
	static char* basePath;

	static const int positivesCount;
	static const int negativesCount;

	static bool positivesLoaded;
	static bool negativesLoaded;

	static vector<Mat> positiveSamples;
	static vector<Mat> negativeSamples;
	static void loadPositiveSamples();
	static void loadNegativeSamples();
	
public:
	Loader(void){}
	~Loader(void){}
	static void setBasePath(char* path);

	static vector<Mat> getPositiveSamples();
	static vector<Mat> getNegativeSamples();
};

