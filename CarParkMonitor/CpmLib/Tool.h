#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class Tool
{
public:
	Tool(void);
	~Tool(void);

	static void rectToCenter(const Point& center, Rect& r);
	static Point rectCenter(const Rect& r);
	static bool rectInside(const Rect& r, const Rect& container);
};

