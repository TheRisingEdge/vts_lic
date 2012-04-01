#include "Draw.h"


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
