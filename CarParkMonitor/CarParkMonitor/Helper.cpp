#include "Helper.h"
#include "Draw.h"
#include <iostream>

using namespace std;

#pragma region storage

cv::Mat Helper::concatImages( Mat img1, Mat img2 )
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

void Helper::drawText( const char* text,const Point& origin, Mat& output, Scalar color)
{	
	putText( output, text, origin, CV_FONT_HERSHEY_PLAIN, 1.2, color);
}

void Helper::drawBlob( const carDetection* b, Mat& output )
{
	Rect rect = b->rect;
	Scalar color = Scalar( 0, 0, 255 );
	rectangle( output, rect.tl(), rect.br(), color);
	
	unique_ptr<char> text = unique_ptr<char>(new char[50]);
	auto data = text.get();
	sprintf(data, "%d", b->id);
	drawText(data,b->rect.tl(), output);
}

void Helper::drawRect( const Rect& r, Mat& output, Scalar color)
{
	rectangle( output, r.tl(), r.br(), color);
}

void Helper::drawAnotatedRect(int nr, const Rect& r, Mat& output, Scalar color )
{
	char* s = new char[20];
	sprintf(s,"%d", nr);
	
	drawRect(r, output, color);
	drawText(s, r.br(), output, color);

	delete[] s;
}

void Helper::drawTracks(vector<track>& tracks, Mat& output, Scalar color )
{
	auto it = tracks.begin();
	auto end = tracks.end();
	for(;it != end; ++it)
	{
		auto rect = it->asRecti();
		drawRect(rect, output, color);

		char* text = new char[50];
		itoa(it->id, text, 10);		
		drawText(text, rect.tl(), output, color);
		delete[] text;
	}		
}

void Helper::drawDetections( const vector<detection>& detections, Mat& image )
{
	auto it = detections.begin();
	auto end = detections.end();
	for(;it != end; ++it)
		drawAnotatedRect(it->id, it->rect, image);		
}


void Helper::drawPoints( const vector<Point> points, Mat& output )
{
	auto it = points.begin();
	auto end = points.end();
	for(; it != end ; ++it)
		cv::circle(output, *it, 2, Scalar(255,0,0));
}

void Helper::drawFPoints( const vector<Point2f> points, Mat& output )
{
	auto it = points.begin();
	auto end = points.end();
	for(; it != end ; ++it)
		cv::circle(output, *it, 2, Scalar(255,0,0));
}

void Helper::drawRectF( const Rect_<float>& r, Mat& output )
{
	Scalar color = Scalar( 0, 255, 0 );
	rectangle( output, r.tl(), r.br(), color);
}

#pragma endregion converters


