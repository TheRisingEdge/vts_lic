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

float Tool::median( float* A, int length )
{
	int index = (int)(length / 2);
	std::sort(A, A + length);

	if (length % 2 == 0) {
		return (A[index] + A[index + 1]) / 2;
	}
	else {
		return A[index];
	}
}

void Tool::moveRect( Rect& r, float x, float y )
{
	auto tl = r.tl();
	x = floor(x + 0.49);
	y = floor(y + 0.49);
	r = Rect(tl.x + x, tl.y + y, r.width, r.height);
}

void Tool::moveRectf( Rect_<float>& r, float x, float y )
{
	auto tl = r.tl();
	r = Rect(tl.x + x, tl.y + y, r.width, r.height);
}

void Tool::scaleRectf( Rect_<float>& r, float xscale, float yscale )
{
	Point2f center = r.tl() + Point2f(r.width/2, r.height/2);
	r = Rect_<float>(0,0, r.width*xscale, r.height*yscale);
	toCenterf(center, r);
}

void Tool::scaleRect( Rect& r, float xscale, float yscale )
{
	Point center = r.tl() + Point(r.width/2, r.height/2);
	r = Rect(0,0, r.width*xscale, r.height*yscale);
	toCenter(center, r);
}


void Tool::toCenterf(const Point2f& center, Rect_<float>& r)
{
	float width = r.width;
	float height = r.height;

	Point2f tl = Point2f(center.x - width/2, center.y - height/2);
	r = Rect(tl.x, tl.y, width, height);	
}

int Tool::rectArea( Rect r )
{		
	if(r.width < 0 || r.height < 0)
		return 0;

	return r.width*r.height;
}

