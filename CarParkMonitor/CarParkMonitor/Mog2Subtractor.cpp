#include "Mog2Subtractor.h"


Mog2Subtractor::Mog2Subtractor(void)
{
	bgModel = BackgroundSubtractorMOG();
	//bgModel.nShadowDetection = 127;
}


Mog2Subtractor::~Mog2Subtractor(void)
{
}

void Mog2Subtractor::learn( const Mat& frame )
{
	bgModel(frame, foreground, -1);
}

cv::Mat Mog2Subtractor::segment( const Mat& frame )
{
	if( maskedForeground.empty() )
		maskedForeground.create(frame.size(), frame.type());

	//update the model
	bgModel(frame, foreground, -1);

	maskedForeground = Scalar::all(0);
	frame.copyTo(maskedForeground, foreground);

	Mat background;
	bgModel.getBackgroundImage(background);

	imshow("image", frame);
	imshow("foreground mask", foreground);
	imshow("foreground image", maskedForeground);
	if(!background.empty())
		imshow("mean background image", background );

	return foreground;
}
