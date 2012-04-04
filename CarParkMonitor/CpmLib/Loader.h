#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Loader
{
private:
	static unique_ptr<char> positivesPath;
	static unique_ptr<char> negativesPath;

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
	~Loader(void){}

	static vector<Mat> getPositiveSamples();
	static vector<Mat> getNegativeSamples();

	static void setPosPathAndCount(char* path, int posCount)
	{
		positivesPath = unique_ptr<char>(path);
		positivesCount = posCount;
	}
	static void setNegPathAndCount(char* path, int negCount)
	{
		negativesPath = unique_ptr<char>(path);
		negativesCount = negCount;
	}
};

