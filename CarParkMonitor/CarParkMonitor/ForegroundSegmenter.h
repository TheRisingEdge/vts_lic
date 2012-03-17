#include "opencv2\opencv.hpp"

using namespace cv;

#define BG_DRAW 0

class ForegroundSegmenter
{
private:
	cv::Mat gray;			// current gray-level image
	cv::Mat background;		// accumulated background
	cv::Mat backImage;		// background image
	cv::Mat foreground;		// foreground image
	Mat newForeground;
	double learningRate;    // learning rate in background accumulation
	int threshold;			// threshold for foreground extraction
	IplImage foregroundIpl;
	
	int closeHolesCount;
	Mat structuringElement;
	Mat elipseElement;
public:
	ForegroundSegmenter();
	~ForegroundSegmenter();

	virtual void learn(Mat frame){};
	virtual Mat segment(Mat frame);
};