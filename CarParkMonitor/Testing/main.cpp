#include <opencv2\opencv.hpp>

using namespace cv;

int main()
{
	Mat img;
	VideoCapture cap(0);

	while (true)
	{
		cap >> img;

		Mat edges;
		cvtColor(img, edges, CV_BGR2GRAY);

		Canny(edges, edges, 30, 60);

		imshow("window label", edges);
		waitKey(1);
	}

	return 0;
}