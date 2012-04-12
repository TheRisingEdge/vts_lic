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

	static Mat concatImages(Mat img1, Mat img2);
	static void drawText(const char* text,const Point& origin, Mat& output);
	static void drawBlob(const carDetection* b, Mat& output);
	static void drawRect(const Rect& r, Mat& output);
	static void drawAnotatedRect(int nr,const Rect& r, Mat& output);
	static void drawPoints(const vector<Point> points, Mat& output);
	static void drawTracks(const vector<track>& tracks, Mat& image);
	static void drawDetections(const vector<detection>& detections, Mat& image);
};
