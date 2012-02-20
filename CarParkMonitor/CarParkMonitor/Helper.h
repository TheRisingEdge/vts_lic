#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class Helper
{
public:
	Helper(void);
	~Helper(void);

	static float** MatToFloats(Mat_<float> m);
	static Mat_<float> floatsToMat(float** data, int cols, int rows);

	static void writeMatVec( FileStorage f, const char* baseKey, vector<Mat> carDescriptors );
	static void readMatVec( FileStorage f, const char* baseKey, vector<Mat>* carDescriptors );
	static void writeKeypointCollectionVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>> carKeypoints );
	static void readKeypointCollectionVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>>* carKeypoints );
	static Mat_<float> filterColumns(Mat_<float> origin, int* columnsKept, int count);
	static vector<int> intArrayToVector(int* arr, int count);
	static int* vectorToIntArray(vector<int> vec, int* count = NULL);
};

