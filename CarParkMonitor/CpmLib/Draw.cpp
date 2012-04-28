#include "Draw.h"
using namespace std;

Draw::Draw(void)
{
}


Draw::~Draw(void)
{
}

void Draw::rect( const Rect& r, Mat& image , Scalar color)
{
	rectangle( image, r.tl(), r.br(), color);
}

void Draw::rects( const vector<Rect>& rects, Mat& image, Scalar color /*= Scalar(0,255,0)*/ )
{
	auto it = rects.begin();
	auto end = rects.end();
	for(;it != end; it++)
	{
		Draw::rect(*it, image, color);
	}
}

void Draw::rectf( const Rect_<float>& r, Mat& image, Scalar color /*= Scalar(0,255,0)*/ )
{
	rectangle( image, r.tl(), r.br(), color);
}
