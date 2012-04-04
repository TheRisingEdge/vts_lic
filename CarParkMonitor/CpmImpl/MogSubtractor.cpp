#include "MogSubtractor.h"


MogSubtractor::MogSubtractor()
{
	init("mog-subtractor");
}

MogSubtractor::MogSubtractor( char* windowName ):BgSubtractorBase(windowName)
{
	init(windowName);
}


void MogSubtractor::learn( const Mat& frame )
{
	bgModel(frame, foreground, -1);
}

cv::Mat MogSubtractor::segment( const Mat& frame )
{
	bgModel(frame, foreground, -1);

	Mat background;
	bgModel.getBackgroundImage(background);
	

	cv::erode(foreground, foreground, smallCrossKernel, Point(-1,-1),2);
	cv::dilate(foreground, foreground, crossKernel, Point(-1,-1),3);
	//cv::dilate(foreground, foreground, elipseKernel, Point(-1,-1),1);

	//cv::erode(foreground, foreground, crossKernel, Point(-1,-1),1);

	//cv::dilate(foreground, foreground, elipseKernel, Point(-1,-1),1);
	
	//cv::dilate(foreground, foreground, elipseKernel, Point(-1,-1),1);	
	//cv::erode(foreground, foreground, elipseKernel, Point(-1,-1),2);	

	//imshow(foregroundWindowName, foreground);
	
	//if(!background.empty())
	//	imshow(meanWindowName, background );

	return foreground;
}

void MogSubtractor::init( char* windowName )
{
	BgSubtractorBase::init(windowName);
	bgModel = BackgroundSubtractorMOG2();
	bgModel.nShadowDetection = 0;

	meanWindowName = new char[50];
	sprintf(meanWindowName, "mog-mean-%d", instanceNr);

	foregroundWindowName = new char[50];
	sprintf(foregroundWindowName,"mog-foreground-%d", instanceNr);


	
	int erosionSize = 2;
	elipseKernel = getStructuringElement( 
		MORPH_ELLIPSE,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
		);

	erosionSize = 2;
	rectKernel = getStructuringElement( 
		MORPH_RECT,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);

	erosionSize = 2;
	crossKernel = getStructuringElement( 
		MORPH_CROSS,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);

	erosionSize = 1;
	smallCrossKernel = getStructuringElement( 
		MORPH_CROSS,
		Size( 2*erosionSize + 1, 2*erosionSize+1 ),
		Point( erosionSize, erosionSize) 
	);

}

cv::Mat MogSubtractor::getBackground()
{
	Mat background;
	bgModel.getBackgroundImage(background);
	return background;
}
