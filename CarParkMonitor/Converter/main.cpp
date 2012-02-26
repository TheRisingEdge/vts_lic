#include "opencv2/opencv.hpp"

using namespace cv;

typedef struct 
{
	char* aviIn;
	char* aviOut;
	int heightOut;
	int widthOut;

}ComParam;

ComParam parseCommandLine(int argc, char** argv)
{
	ComParam param;

	int inIndex = 1;
	int outIndex = 2;
	int widthIndex = 3;
	int heightIndex = 4;

	param.aviIn = argv[inIndex];
	param.aviOut = argv[outIndex];
	param.widthOut = atoi(argv[widthIndex]);
	param.heightOut = atoi(argv[heightIndex]);

	return param;
}

int main(int argc, char** argv)
{	

	ComParam param = parseCommandLine(argc, argv);

	VideoCapture capture(param.aviIn); // open the default camera
	if( !capture.isOpened() )  {
		printf("Cannot open video file!\n");
		return -1;
	}

	Size outSize = Size(param.widthOut, param.heightOut);	
	VideoWriter record(param.aviOut, CV_FOURCC('D','I','V','X'), 30, outSize, true);
	if( !record.isOpened() ) {
		printf("VideoWriter failed to open!\n");
		return -1;
	}

	namedWindow("video");
	namedWindow("new video");

	Mat frame;
	
	bool stop = false;
	while(!stop)
	{		
		capture >> frame;
		Mat newFrame = Mat(outSize, CV_32FC3);

		cv::resize(frame, newFrame, outSize, 0, 0, INTER_LANCZOS4);
		// show frame on screen
		imshow("video", frame);
		imshow("new video", newFrame);

		// add frame to recorded video
		record << frame; 

		if(waitKey(30) >= 0) break;
	}
	
	return 0;
}