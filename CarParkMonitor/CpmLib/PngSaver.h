#pragma once
#include <opencv2\opencv.hpp>

using namespace cv;

class PngSaver
{
private:
	static char* basePath;
	static int count;

public:
	static void save(char* name, Mat image);
	static void setBasePath(char* basePath);
	static void incrementCount() { count++; }
};

