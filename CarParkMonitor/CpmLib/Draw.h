#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class Draw
{
public:
	Draw(void){}
	~Draw(void){}

	static void rectf(const Rect_<float>& r, Mat& image, Scalar color = Scalar(0,255,0));
	static void rect(const Rect& r, Mat& image, Scalar color = Scalar(0,255,0));
	static void rects(const vector<Rect>& rects, Mat& image, Scalar color = Scalar(0,255,0));

	static Mat concatImages(Mat img1, Mat img2);
	static void points(const vector<Point> points, Mat& output);
	static void pointsF(const vector<Point2f> points, Mat& output);
	static void text(const char* text,const Point& origin, Mat& output, Scalar color = Scalar(0,255,0));	
	static void text(string text,const Point& origin, Mat& output, Scalar color = Scalar(0,255,0));		
	static void anotatedRect(int nr,const Rect& r, Mat& output, Scalar color = Scalar(0,255,0));
};



