#include "Tool.h"


float Tool::Pi = 3.1416;
float Tool::Pi4 = 12.5664;

void Tool::toCenter(const Point& center, Rect& r )
{
	int width = r.width;
	int height = r.height;

	Point tl = Point(center.x - width/2, center.y - height/2);
	r = Rect(tl.x, tl.y, width, height);	
}

Point Tool::rectCenter( const Rect& r )
{
	return Point(r.x + r.width/2, r.y + r.height/2);
}

bool Tool::rectInside( const Rect& r, const Rect& container )
{
	auto tl = r.tl();
	auto br = r.br();

	return container.contains(tl) && container.contains(br);
}

bool Tool::isMostlyCircular( const vector<Point>& contour, float treshold )
{
	Mat hull;
	cv::convexHull(contour, hull);
	double area = cv::contourArea(hull);
	double perimeter = cv::arcLength(hull, true);
	double th = (Pi4*area)/(perimeter*perimeter);
	if(th < treshold)
		return true;

	return false;	
}

void Tool::extend( Rect& r, Point dims )
{
	auto tl = r.tl();
	Point halfSize = Point(dims.x/2, dims.y/2);
	tl = tl - halfSize;

	r = Rect(tl.x, tl.y, r.width + halfSize.x, r.height + halfSize.y);
}


