#pragma once
#include "FrameProcessor.h"
#include <opencv2\opencv.hpp>
#include "BgSubtractorBase.h"
#include "BlobDetector.h"

using namespace std;
using namespace cv;

struct HogFramerParams
{
	BgSubtractorBase* bgSubtractor;
	Rect cropFrame;
};

struct DrawingControl
{
	int x;
	int y;
	bool drawing;
};

struct RegionExtractor
{
	Rect rect;
	BgSubtractorBase* bgSubtractor;
	char* windowName;
};

class HogFramer:public FrameProcessor
{
private: 
	HogFramerParams params;

	BgSubtractorBase* bgSubtractor;
	BlobDetector* blobDetector;
	int trainingFrames;
	int frameCount;
	int posCount;
	int negCount;

	char* nextPositivePath();
	char* nextNegativePath();

	vector<RegionExtractor> regionExtractors;
	void selectMonitorRegions(const Mat& frame);

public:
	static DrawingControl drawingControl;
	static vector<Rect> monitoredRegions;

	HogFramer(HogFramerParams params);
	~HogFramer(void);
	

	void process(const Mat& in);

	void savePositiveAndNegatives( vector<shared_ptr<blob>> blobs,const Rect& crop, const Mat& frame, const Mat& foreground);
	void generateNegativeSamples( const Rect& r, const Mat& frame );
};