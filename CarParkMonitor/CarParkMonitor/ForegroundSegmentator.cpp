#include "ForegroundSegmentator.h"

ForegroundSegmentator::ForegroundSegmentator():threshold(10), learningRate(0.001)
{
	int erosionType = MORPH_RECT;
	int erosionSize = 1;

	this->structuringElement = getStructuringElement( 
		erosionType,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);
}

ForegroundSegmentator::~ForegroundSegmentator(void)
{
}

void ForegroundSegmentator::SaveState( CvFileStorage* )
{

}

void ForegroundSegmentator::LoadState( CvFileStorage* , CvFileNode* )
{

}

void ForegroundSegmentator::ParamUpdate()
{

}


IplImage* ForegroundSegmentator::GetMask()
{	
	return &foregroundIpl;
}

void ForegroundSegmentator::Process( IplImage* pImg )
{
	Mat frame(pImg);
	// convert to gray-level image
	cv::cvtColor(frame, gray, CV_BGR2GRAY); 

	// initialize background to 1st frame
	if (background.empty())
		gray.convertTo(background, CV_32F);

	// convert background to 8U
	background.convertTo(backImage,CV_8U);

	// compute difference between current image and background
	cv::absdiff(backImage,gray,foreground);

	// apply threshold to foreground image
	cv::threshold(foreground, foreground,threshold,255,cv::THRESH_BINARY);

	// accumulate background
	cv::accumulateWeighted(gray, background, learningRate, foreground);	

	erode(foreground, newForeground, structuringElement);
	dilate( newForeground, foreground, structuringElement );
	
	foregroundIpl = foreground;
}

void ForegroundSegmentator::Release()
{

}


