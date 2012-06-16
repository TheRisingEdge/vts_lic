#include "Draw.h"
using namespace std;

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

cv::Mat Draw::concatImages( Mat img1, Mat img2 )
{
	Mat imgResult(std::max(img2.rows,img1.rows),img1.cols+img2.cols,img1.type()); // Your final image

	Mat roiImgResult_Left = imgResult(Rect(0,0,img1.cols,img1.rows)); //Img1 will be on the left part
	Mat roiImgResult_Right = imgResult(Rect(img1.cols,0,img2.cols,img2.rows)); //Img2 will be on the right part, we shift the roi of img1.cols on the right

	Mat roiImg1 = img1(Rect(0,0,img1.cols,img1.rows));
	Mat roiImg2 = img2(Rect(0,0,img2.cols,img2.rows));

	roiImg1.copyTo(roiImgResult_Left); //Img1 will be on the left of imgResult
	roiImg2.copyTo(roiImgResult_Right); //Img2 will be on the right of imgResult

	return imgResult;
}

void Draw::text( const char* text,const Point& origin, Mat& output, Scalar color)
{	
	putText( output, text, origin, CV_FONT_HERSHEY_PLAIN, 1.2, color);
}

void Draw::text( string text,const Point& origin, Mat& output, Scalar color /*= Scalar(0,255,0)*/ )
{
	putText( output, text, origin, CV_FONT_HERSHEY_PLAIN, 1.2, color);
}

void Draw::anotatedRect(int nr, const Rect& r, Mat& output, Scalar color )
{
	char* s = new char[20];
	sprintf(s,"%d", nr);

	Draw::rect(r, output, color);
	text(s, r.br(), output, color);

	delete[] s;
}

void Draw::points( const vector<Point> points, Mat& output )
{
	auto it = points.begin();
	auto end = points.end();
	for(; it != end ; ++it)
		cv::circle(output, *it, 2, Scalar(255,0,0));
}

void Draw::pointsF( const vector<Point2f> points, Mat& output )
{
	auto it = points.begin();
	auto end = points.end();
	for(; it != end ; ++it)
		cv::circle(output, *it, 2, Scalar(255,0,0));
}