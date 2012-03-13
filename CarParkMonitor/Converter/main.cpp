#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

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

	cout << "press [r] to start recording, [e] to finish" << endl;

	namedWindow("video");
	namedWindow("new video");

	Mat frame;
	
	bool stop = false;
	bool recording = false;
	bool finished = false;

	while(!stop)
	{		
		capture >> frame;


		if(!recording && !finished)
		{
			if(cv::waitKey(1) == 'r')
			{
				recording = true;
			}
		}else{
			if(cv::waitKey(1) == 'e')
			{
				recording = false;
				finished = true;
			}
		}
		
		if(recording && !finished)
		{
			Mat newFrame = Mat(outSize, CV_32FC3);
			cv::resize(frame, newFrame, outSize, 0, 0, INTER_LANCZOS4);
			
			imshow("new video", newFrame);
			// add frame to recorded video
			record << frame; 			
		}

		if(finished)
			break;

		cv::waitKey(5);
		// show frame on screen
		imshow("video", frame);
	}
	
	return 0;
}