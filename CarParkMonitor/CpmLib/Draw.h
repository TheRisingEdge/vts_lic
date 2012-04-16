#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class Draw
{
public:
	Draw(void);
	~Draw(void);

	static void rectf(const Rect_<float>& r, Mat& image, Scalar color = Scalar(0,255,0));
	static void rect(const Rect& r, Mat& image, Scalar color = Scalar(0,255,0));
	static void rectVector(const vector<Rect>& rects, Mat& image, Scalar color = Scalar(0,255,0));
};



