#include "ForegroundSegmentator.h"

ForegroundSegmentator::ForegroundSegmentator():threshold(10), learningRate(0.0001)
{
	this->closeHolesCount = 1;

	int erosionType = MORPH_CROSS;
	int erosionSize = 2;


	this->structuringElement = getStructuringElement( 
		erosionType,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);

	this->elipseElement = getStructuringElement( 
		MORPH_RECT,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);
}

Mat ForegroundSegmentator::segment( Mat frame )
{
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
	cv::accumulateWeighted(gray, background, learningRate, foreground);	//can also pass mask to ignore stationary cars

	

	//cv::dilate(foreground, foreground, structuringElement);
	//cv::dilate(foreground, foreground, structuringElement);
	//
	//cv::erode(foreground, foreground, structuringElement);
	//cv::erode(foreground, foreground, structuringElement);
	//cv::erode(foreground, foreground, structuringElement);
	
	


	if (this->closeHolesCount > 0){
		Mat temp_foreground_mask;

		// close holes
		cv::erode(foreground, temp_foreground_mask, structuringElement, Point(-1,-1), this->closeHolesCount);
		cv::dilate(temp_foreground_mask, foreground, structuringElement, Point(-1,-1), this->closeHolesCount);

		// close gaps
		cv::dilate(temp_foreground_mask, foreground, structuringElement, Point(-1,-1), this->closeHolesCount);
		cv::erode(foreground, temp_foreground_mask, structuringElement, Point(-1,-1), this->closeHolesCount);
		cv::dilate(temp_foreground_mask, foreground, this->elipseElement, Point(-1,-1), this->closeHolesCount);
	}
	
	imshow("Segmentator", foreground);	

	return foreground;
}



