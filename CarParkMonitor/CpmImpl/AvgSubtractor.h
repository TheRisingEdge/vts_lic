#include "BgSubtractorBase.h"
#include "opencv2\opencv.hpp"

using namespace cv;

#define BG_DRAW 0

class AvgSubtractor: public BgSubtractorBase
{
private:
	cv::Mat grayFrame;			// current gray-level image
	cv::Mat background;		// accumulated background
	cv::Mat backgroundGray;		// background image
	cv::Mat foreground;		// foreground image
	cv::Mat newForeground;

	double learningRate;    // learning rate in background accumulation
	int threshold;			// threshold for foreground extraction

	int closeHolesCount;
	Mat structuringElement;
	Mat elipseElement;

	void init(char* windowName);

public:
	AvgSubtractor();
	AvgSubtractor(char* windowName);
	~AvgSubtractor();

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
};

