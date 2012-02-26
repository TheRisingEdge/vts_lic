
#include "opencv2\opencv.hpp"
#include "opencv2\legacy\blobtrack.hpp"
#include "opencv2\core\core.hpp"

using namespace cv;


class ForegroundSegmentator : public CvFGDetector
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
	Mat structuringElement;

public:
	ForegroundSegmentator();
	~ForegroundSegmentator();

	virtual void SaveState(CvFileStorage* );;
	virtual void LoadState(CvFileStorage* , CvFileNode* );;
		
	void ParamUpdate();
	IplImage* GetMask();;

	/* Process current image: */
	virtual void  Process(IplImage* pImg);
	/* Release foreground detector: */
	virtual void    Release();;
};