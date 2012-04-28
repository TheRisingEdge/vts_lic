#include <iostream>
#include <vector>

//#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
//#include <opencv2/highgui/highgui_c.h>

using namespace cv;
using namespace std;
bool onlyPred = false;

struct mouse_info_struct { int x,y; };
struct mouse_info_struct mouse_info = {-1,-1}, last_mouse;

vector<Point> mousev,kalmanv;

void on_mouse(int event, int x, int y, int flags, void* param) {
	//if (event == CV_EVENT_LBUTTONUP) 
	{
		last_mouse = mouse_info;
		mouse_info.x = x;
		mouse_info.y = y;

		//		cout << "got mouse " << x <<","<< y <<endl;
	}
}

// plot points
#define drawCross( rectCenter, color, d )                                 \
	line( img, Point( rectCenter.x - d, rectCenter.y - d ),                \
	Point( rectCenter.x + d, rectCenter.y + d ), color, 2, CV_AA, 0); \
	line( img, Point( rectCenter.x + d, rectCenter.y - d ),                \
	Point( rectCenter.x- d, rectCenter.y + d ), color, 2, CV_AA, 0 )


int main (int argc, char * const argv[]) {
	Mat img(500, 500, CV_8UC3);

	KalmanFilter KF(6,2);
	Mat_<float> state(6,1); /* (x, y, Vx, Vy) */

	Mat processNoise(6, 1, CV_32F);
	
	Mat_<float> measurement(2,1);/* [x, y] (t) */
	measurement.setTo(Scalar(0));
	
	char code = (char)-1;

	namedWindow("mouse kalman");
	setMouseCallback("mouse kalman", on_mouse, 0);

	for(;;)
	{
		if (mouse_info.x < 0 || mouse_info.y < 0) {
			imshow("mouse kalman", img);
			waitKey(30);
			continue;
		}
		
		KF.statePost.at<float>(0) = mouse_info.x;
		KF.statePost.at<float>(1) = mouse_info.y;
		KF.statePost.at<float>(2) = 0;
		KF.statePost.at<float>(3) = 0;
		KF.statePost.at<float>(4) = 0;
		KF.statePost.at<float>(5) = 0;

		KF.transitionMatrix = (Mat_<float>(6, 6) << 
			1,0,1,0,0.5,0,
			0,1,0,1,0,0.5,
			0,0,1,0,1,0,
			0,0,0,1,0,1,
			0,0,0,0,1,0,
			0,0,0,0,0,1);

		KF.measurementMatrix = (Mat_<float>(2,6) <<
			1,0,0,0,0,0,
			0,1,0,0,0,0
		);


		setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
		setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
		setIdentity(KF.errorCovPost, Scalar::all(.1));

		mousev.clear();
		kalmanv.clear();
		Point lastPrediction;
		
		for(;;)
		{
			//            Point statePt(state(0),state(1));

			if(onlyPred)
			{
				int a;
				a = 34;
			}

			KF.predict(); //PREDICTION
			Mat prediction = KF.statePre;
			Point predictPt(prediction.at<float>(0),prediction.at<float>(1));
			float vx = prediction.at<float>(2);
			float vy = prediction.at<float>(3);

			lastPrediction = predictPt;
	
			if(!onlyPred)
			{
				measurement(0) = mouse_info.x; //MEASUREMENT
				measurement(1) = mouse_info.y;

				Point measPt(measurement(0),measurement(1));
				mousev.push_back(measPt);

				Mat estimated = KF.correct(measurement);//CORRECT
				Point statePt(estimated.at<float>(0),estimated.at<float>(1));
				kalmanv.push_back(statePt);
				drawCross( statePt, Scalar(255,255,255), 5 );
				line( img, statePt, measPt, Scalar(0,0,255), 3, CV_AA, 0 );
				line( img, statePt, predictPt, Scalar(0,255,255), 3, CV_AA, 0 );

				drawCross( measPt, Scalar(0,0,255), 5 );
			}
			img = Scalar::all(0);
			drawCross( predictPt, Scalar(0,255,0), 3 );
			
			
	

			for (int i = 0; i < mousev.size()-1; i++) {
				line(img, mousev[i], mousev[i+1], Scalar(255,255,0), 1);
			}
			for (int i = 0; i < kalmanv.size()-1; i++) {
				line(img, kalmanv[i], kalmanv[i+1], Scalar(0,255,0), 1);
			}


			imshow( "mouse kalman", img );
			code = (char)waitKey(100);
			if(code == 'r')
				onlyPred = !onlyPred;
		}
		if( code == 27 || code == 'q' || code == 'Q' )
			break;
	}

	return 0;
}