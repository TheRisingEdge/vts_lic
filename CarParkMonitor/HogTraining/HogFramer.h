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
	int id;
	Rect rect;
	BgSubtractorBase* bgSubtractor;
	BlobDetector* detector;
	char* windowName;
	char* saveWindowName;
};

class HogFramer:public FrameProcessor
{
private:
	BgSubtractorBase* bgSubtractor;
	BlobDetector* blobDetector;
	int trainingFrames;
	int frameCount;

	int posCount;
	int negCount;
	int whiteCount;
	int blackCount;

	static int** pathCounters;

	unique_ptr<char> nextNegPath(int regId = 0);
	unique_ptr<char> nextWPath(int regId = 0);
	unique_ptr<char> nextBPath(int regId = 0);
	
	vector<RegionExtractor> regionExtractors;
	void selectMonitorRegions(const Mat& frame);	
	void generateNegativeSamples( const Rect& r, const Mat& frame, int regId = 0);

	void savePositive(const Mat& regionOnRoi, const Mat& regionOnBackground, int regId = 0);
	void savePositiveAndNegatives( vector<blob> blobs,const Rect& crop,const Mat& roi, const Mat& roiForeground, const Mat& frame, int regId = 0);
	void initPathCounters(int len);
	void saveLastRunConfig();

public:
	static DrawingControl drawingControl;
	static vector<Rect> monitoredRegions;

	HogFramer();
	~HogFramer(void){}
	
	void process(const Mat& in);	
};