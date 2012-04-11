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

void Draw::rectVector( const vector<Rect>& rects, Mat& image, Scalar color /*= Scalar(0,255,0)*/ )
{
	auto it = rects.begin();
	auto end = rects.end();
	for(;it != end; it++)
	{
		Draw::rect(*it, image, color);
	}
}
