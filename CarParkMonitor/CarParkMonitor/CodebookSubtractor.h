#pragma once
#include "BgSubtractorBase.h"

using namespace cv;

class CodebookSubtractor: public BgSubtractorBase
{
private:
	bool createdResources;
	bool stateCleared;
	/*std::unique_ptr<CvBGCodeBookModel> model;
	std::unique_ptr<IplImage> yuvImage;
	std::unique_ptr<IplImage> ImaskCodeBook;
	std::unique_ptr<IplImage> ImaskCodeBookCC;*/
	
	CvBGCodeBookModel* model;
	IplImage* yuvImage;
	IplImage* ImaskCodeBook;
	IplImage* ImaskCodeBookCC;

public:
	CodebookSubtractor(void);
	~CodebookSubtractor(void);

	void learn(const Mat& frame);
	Mat segment(const Mat& frame);
};

