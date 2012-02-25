#include "BackgroundSegmentator.h"

BackgroundSegmentator::BackgroundSegmentator( void ):FrameProcessor("Mog")
{

}

void BackgroundSegmentator::process( cv::Mat in, cv::Mat* out )
{
	mog(in, *out, 0.0075);	
	cv::threshold(*out,*out,
		120,200,cv::THRESH_BINARY_INV);
}


