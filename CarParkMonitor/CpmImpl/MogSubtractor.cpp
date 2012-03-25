#include "MogSubtractor.h"


MogSubtractor::MogSubtractor(void)
{
	bgModel = BackgroundSubtractorMOG2();
	bgModel.nShadowDetection = 0;
}


MogSubtractor::~MogSubtractor(void)
{
}

void MogSubtractor::learn( const Mat& frame )
{
	bgModel(frame, foreground, -1);
}

cv::Mat MogSubtractor::segment( const Mat& frame )
{
	if( maskedForeground.empty() )
		maskedForeground.create(frame.size(), frame.type());

	//update the model
	bgModel(frame, foreground, -1);

	maskedForeground = Scalar::all(0);
	//frame.copyTo(maskedForeground, foreground);

	Mat background;
	bgModel.getBackgroundImage(background);

	//imshow("image", frame);
	imshow("foreground mask", foreground);
	//imshow("foreground image", maskedForeground);
	if(!background.empty())
		imshow("mean background image", background );

	return foreground;
}
