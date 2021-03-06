#include "AvgSubtractor.h"
#include "PngSaver.h"

void AvgSubtractor::init( char* windowName )
{
	this->threshold = 20;
	this->learningRate = 0.4;

	this->closeHolesCount = 1;
	int erosionType = MORPH_CROSS;
	int erosionSize = 3;

	this->structuringElement = getStructuringElement( 
		erosionType,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);
}

AvgSubtractor::AvgSubtractor():BgSubtractorBase()
{
	init("avg-sub");
}

AvgSubtractor::AvgSubtractor( char* windowName ):BgSubtractorBase(windowName)
{
	init(windowName);
}

void AvgSubtractor::learn(const Mat& frame)
{
	segment(frame);
}

Mat AvgSubtractor::segment(const Mat& frame )
{
	// convert to gray-level image
	cv::cvtColor(frame, grayFrame, CV_BGR2GRAY); 

	// initialize background to 1st frame
	if (background.empty())
		grayFrame.convertTo(background, CV_32F);

	// convert background to 8U
	background.convertTo(backgroundGray,CV_8U);

	// compute difference between current image and background
	cv::absdiff(backgroundGray,grayFrame,foreground);

	// apply threshold to foreground image
	cv::threshold(foreground, foreground,threshold,255,cv::THRESH_BINARY);

	// accumulate background
	cv::accumulateWeighted(grayFrame, background, learningRate, foreground);	//can also pass mask to ignore stationary cars

	/*PngSaver::save("before", foreground);
	if (this->closeHolesCount > 0)
	{
	Mat temp_foreground_mask;

	cv::erode(foreground, temp_foreground_mask, structuringElement, Point(-1,-1), 1);	
	cv::dilate(temp_foreground_mask, foreground, structuringElement, Point(-1,-1),2);		
	cv::erode(foreground, temp_foreground_mask, structuringElement, Point(-1,-1),2);	
	}
	PngSaver::save("after", foreground);*/
	
	return foreground;
}

cv::Mat AvgSubtractor::getBackground()
{
	return this->background;
}




