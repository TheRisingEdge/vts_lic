#include "CodebookSubtractor.h"


CodebookSubtractor::CodebookSubtractor(void)
{
	//model = std::unique_ptr<CvBGCodeBookModel>(cvCreateBGCodeBookModel());
	model = cvCreateBGCodeBookModel();

	//Set color thresholds to default values
	model->modMin[0] = 3;
	model->modMin[1] = model->modMin[2] = 3;

	model->modMax[0] = 10;
	model->modMax[1] = model->modMax[2] = 10;

	model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = 10;

	createdResources = false;
	stateCleared = false;
}


CodebookSubtractor::~CodebookSubtractor(void)
{
}

void CodebookSubtractor::learn( const Mat& frame )
{
	// CODEBOOK METHOD ALLOCATION
	if(!createdResources)
	{
		//IplImage rawImage = frame;
		//yuvImage = std::unique_ptr<IplImage>(cvCloneImage(&rawImage));

		//ImaskCodeBook = std::unique_ptr<IplImage> (cvCreateImage( cvGetSize(&rawImage), IPL_DEPTH_8U, 1 ));
		//ImaskCodeBookCC = std::unique_ptr<IplImage>(cvCreateImage( cvGetSize(&rawImage), IPL_DEPTH_8U, 1 ));
		//cvSet(ImaskCodeBook.get(),cvScalar(255));

		//cvNamedWindow( "Raw", 1 );
		//cvNamedWindow( "ForegroundCodeBook",1);
		//cvNamedWindow( "CodeBook_ConnectComp",1);

		IplImage img = frame;
		IplImage* rawImage = &img;

		yuvImage = cvCloneImage(rawImage);
		ImaskCodeBook = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
		ImaskCodeBookCC = cvCreateImage( cvGetSize(rawImage), IPL_DEPTH_8U, 1 );
		cvSet(ImaskCodeBook,cvScalar(255));

		cvNamedWindow( "Raw", 1 );
		cvNamedWindow( "ForegroundCodeBook",1);
		cvNamedWindow( "CodeBook_ConnectComp",1);

		createdResources = true;
	}

	//cvBGCodeBookUpdate( model.get(), yuvImage.get() );
	cvBGCodeBookUpdate( model, yuvImage);
}

cv::Mat CodebookSubtractor::segment( const Mat& frame )
{
	if(!stateCleared)
	{
		cvBGCodeBookClearStale( model, model->t/2 );
		stateCleared = true;
	}
             
	IplImage rai = frame.clone();
	IplImage* rawImage = &rai;
	
	cvCvtColor( rawImage, yuvImage, CV_BGR2YCrCb );//YUV For codebook method

	// Find foreground by codebook method
	cvBGCodeBookDiff( model, yuvImage, ImaskCodeBook);
	// This part just to visualize bounding boxes and centers if desired
	cvCopy(ImaskCodeBook,ImaskCodeBookCC);	
	cvSegmentFGMask( ImaskCodeBookCC);

	cvShowImage( "Raw", rawImage );
	cvShowImage( "ForegroundCodeBook",ImaskCodeBook);
	cvShowImage( "CodeBook_ConnectComp",ImaskCodeBookCC);
	
	Mat foreground(ImaskCodeBook);
	return foreground;
}
