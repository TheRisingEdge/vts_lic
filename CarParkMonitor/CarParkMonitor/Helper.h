#pragma once
#include <opencv2/opencv.hpp>
#include "Blob.h"

using namespace cv;

class Helper
{
public:
	Helper(void);
	~Helper(void);

	static Mat_<float> filterColumns(Mat_<float> origin, int* columnsKept, int count);

	static float** MatToFloats(Mat_<float> m);
	static Mat_<float> floatsToMat(float** data, int cols, int rows);		

	static Mat_<int> intArrayToMat(int* arr, int len);
	static int* MatToIntArray(Mat_<int> mat, int* len);

	static void writeMatVec( FileStorage f, const char* baseKey, vector<Mat> carDescriptors );
	static void readMatVec( FileStorage f, const char* baseKey, vector<Mat>* carDescriptors );

	static void writeKeypointCollectionVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>> carKeypoints );
	static void readKeypointCollectionVec( FileStorage f,const char* sizeKey, const char* baseKey, vector<vector<KeyPoint>>* carKeypoints );
	
	static Mat concatImages(Mat img1, Mat img2);
	static void drawText(const char* text,const Point& origin, Mat& output);
	static void drawBlob(const blob* b, Mat& output);
	static void drawRect(const Rect& r, Mat& output);
	static void drawAnotatedRect(int nr,const Rect& r, Mat& output);
};
