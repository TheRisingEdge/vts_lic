#pragma once
#include <opencv2/opencv.hpp>
#include "Blob.h"
#include "PTracker.h"

using namespace cv;

class Helper
{
public:
	Helper(){};
	~Helper(){};

	static void drawBlob(const carDetection* b, Mat& output);
	static Mat concatImages(Mat img1, Mat img2);
	
	static void drawPoints(const vector<Point> points, Mat& output);
	static void drawFPoints(const vector<Point2f> points, Mat& output);

	static void drawText(const char* text,const Point& origin, Mat& output, Scalar color = Scalar(0,255,0));	
	static void drawRectF(const Rect_<float>& r, Mat& output);
	static void drawRect(const Rect& r, Mat& output, Scalar color = Scalar(0,255,0));
	static void drawAnotatedRect(int nr,const Rect& r, Mat& output, Scalar color = Scalar(0,255,0));
	
	static void drawTracks(vector<track>& tracks, Mat& image, Scalar color = Scalar(255,0,0));
	static void drawDetections(const vector<detection>& detections, Mat& image);	
};
