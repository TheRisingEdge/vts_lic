#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class RectTool
{
private: 
	static float Pi;
	static float Pi4;

	RectTool(void){}
	~RectTool(void){}
public:
	
	static int area(Rect r);
	static void scaleF(Rect_<float>& r, float xscale, float yscale);
	static void scale(Rect& r, float xscale, float yscale);
	static void moveF(Rect_<float>& r, float x, float y);
	static void move(Rect& r, float x, float y);
	static float median(float* arr, int len);
	static void extend(Rect& r, Point dims);
	static void toCenter(const Point& center, Rect& r);
	static void toCenterF(const Point2f& center, Rect_<float>& r);
	static Point center(const Rect& r);
	static bool isContained(const Rect& r, const Rect& container);
	static bool isMostlyCircular(const vector<Point>& contour, float treshold = 0.6f);
};

