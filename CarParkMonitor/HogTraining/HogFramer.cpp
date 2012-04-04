#include "HogFramer.h"
#include "Content.h"
#include "Draw.h"
#include "Tool.h"
#include <math.h>
#include <array>
#include <ppl.h>
#include <concurrent_vector.h>
#include "AvgSubtractor.h"
#include "MogSubtractor.h"
#include "StableAvgSubtractor.h"
#include "StableMog.h"

using namespace Concurrency;

DrawingControl HogFramer::drawingControl;
vector<Rect> HogFramer::monitoredRegions;

static const double pi = 3.1416;	
const char* frameWindowName = "frame";

static const int width = 80;
static const int height = 64;

static const int cropHeight = 90;

static const int NEG_INDEX = 0;
static const int POSW_INDEX = 1;
static const int POSB_INDEX = 2;

int** HogFramer::pathCounters;


bool isNonCircular(shared_ptr<blob> b)
{	
	if(!b || b->contour.size() < 20)
		return true;

	return Tool::isMostlyCircular(b->contour);
}

HogFramer::HogFramer( HogFramerParams params ):FrameProcessor("Hog Extractor")
{
	this->params = params;
	this->blobDetector = new BlobDetector(30, 30, "detector");

	HogFramer::drawingControl.drawing = false;

	frameCount = 0;
	trainingFrames = 100;
	posCount = 0;
	negCount = 0;
	whiteCount = 0;
	blackCount = 0;
}

void HogFramer::process( const Mat& frame )
{
	Mat frameClone = frame.clone();	

	frameCount++;
	if(frameCount == 4)
	{
		selectMonitorRegions(frame);
		initPathCounters(monitoredRegions.size());
		for(int i = 0; i < monitoredRegions.size(); i++)
		{
			RegionExtractor ext;			

			char* detectorName = new char[50];
			char* windowName = new char[50];
			char* saveWindowName = new char[50];

			sprintf(detectorName, "detector %d", i);
			sprintf(windowName, "window %d", i);
			sprintf(saveWindowName, "saving %d", i);

			ext.id = i;
			ext.rect = monitoredRegions[i];
			ext.bgSubtractor = new MogSubtractor();
			ext.detector = new BlobDetector(30,30,detectorName);
			ext.windowName = windowName;
			ext.saveWindowName = saveWindowName;

			regionExtractors.push_back(ext);
		}

	}else
	{
		for_each(begin(regionExtractors), end(regionExtractors), [&](const RegionExtractor& ext){
			Draw::rect(ext.rect, frameClone);
		});

		parallel_for_each(regionExtractors.begin(), regionExtractors.end(), [&](const RegionExtractor& ext)
		{			
			Mat roi = frame(ext.rect);
			//imshow(ext.windowName, roi);

			if(frameCount < trainingFrames)
			{
				ext.bgSubtractor->learn(roi);

			}else
			{
				Mat foreground = ext.bgSubtractor->segment(roi);
				Mat background = ext.bgSubtractor->getBackground();

				DetectorParams detParam;
				detParam.foreground = foreground;
				detParam.frame = roi;				
				vector<shared_ptr<blob>> blobs =  ext.detector->detect(detParam);
				
				std::remove_if(begin(blobs), end(blobs),isNonCircular);				
				savePositiveAndNegatives(blobs,ext.rect, roi, background, frame, ext.id);
			}
		});
	}

	imshow(frameWindowName, frameClone);
	frameClone.release();
}

void HogFramer::savePositiveAndNegatives( vector<shared_ptr<blob>> blobs,const Rect& crop,const Mat& roi, const Mat& roiBackground ,const Mat& frame, int regId)
{
	for_each(begin(blobs), end(blobs), [&](shared_ptr<blob> b)
	{				
		if(b){
			Point cropTopLeft = crop.tl();
			Rect blobRect = b->rect;
			Point centerOnCrop = Tool::rectCenter(blobRect);
			Point centerOnFrame = cropTopLeft + centerOnCrop;

			Rect cropRect = Rect(0,0, width, height);
			Tool::toCenter(centerOnFrame, cropRect);

			if(Tool::rectInside(cropRect, crop))
			{
				Point centerOnRoi = Tool::rectCenter(blobRect);
				Rect rectOnRoi = Rect(0, 0, width, height);
				Tool::toCenter(centerOnRoi, rectOnRoi);

				Mat regionOnRoi = roi(rectOnRoi);
				//Mat regionOnBackground = roiBackground(rectOnRoi);

				//savePositive(regionOnRoi, regionOnBackground);
				savePositive(regionOnRoi, Mat(), regId);

				//imshow("saving", regionOnRoi);
				//imshow("bb", regionOnBackground);

				//generating negative samples centered in the corners of the car detection
				generateNegativeSamples(cropRect, frame);		
			}		
		}		
	});
}

void HogFramer::generateNegativeSamples( const Rect& r, const Mat& frame, int regId)
{
	static Rect frameRect(0,0, frame.cols, frame.rows);

	Rect topLeft = Rect(0,0,width, height);
	Tool::toCenter(r.tl(), topLeft);

	Rect topRight(0,0, width, height);
	Point p = r.tl() + Point(width, 0);
	Tool::toCenter(p, topRight);
	

	Rect bottomLeft = Rect(0,0, width, height);
	p = r.tl() - Point(0, height);
	Tool::toCenter(p, bottomLeft);

	Rect bottomRight = Rect(0,0, width, height);
	p = r.br();
	Tool::toCenter(p, bottomRight);

	unique_ptr<char> path;
	if(Tool::rectInside(topLeft,frameRect))
	{
		path = nextNegPath(regId);
		imwrite(path.get(), frame(topLeft));		
	}
	
	/*if(Tool::rectInside(topRight,frameRect))
	{
	path = nextNegPath(regId);
	imwrite(path.get(), frame(topRight));
	}

	if(Tool::rectInside(bottomLeft, frameRect))
	{
	path = nextNegPath(regId);
	imwrite(path.get(), frame(bottomLeft));
	}*/
	
	if(Tool::rectInside(bottomRight, frameRect))
	{
		path = nextNegPath(regId);
		imwrite(path.get(), frame(bottomRight));
	}	
}

void onMouseClick(int event, int x, int y, int flags, void* param)
{
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:	
		HogFramer::drawingControl.drawing = true;
		HogFramer::drawingControl.x = x;
		HogFramer::drawingControl.y = y;
		break;
	
	case CV_EVENT_LBUTTONUP:
		if(HogFramer::drawingControl.drawing)
		{
			int width = abs(HogFramer::drawingControl.x - x);
			int h = cropHeight;//abs(HogFramer::drawingControl.y - y);
			Rect recti = Rect(HogFramer::drawingControl.x, HogFramer::drawingControl.y, width, h);
			HogFramer::monitoredRegions.push_back(recti);
			HogFramer::drawingControl.drawing = false;
		}		
		break;

	default:
		break;
	}
}
void HogFramer::selectMonitorRegions(const Mat& frame)
{
	Mat clone = frame.clone();
	const char* name = frameWindowName;
	imshow(name, clone);

	cv::setMouseCallback(name,onMouseClick);

	while(true)
	{
		for_each(begin(HogFramer::monitoredRegions), end(HogFramer::monitoredRegions), [&](Rect re){
			Draw::rect(re, clone);
			imshow(name, clone);
		});	

		if(cv::waitKey(100) > 0)
			break;
	}

	clone.release();
	cv::setMouseCallback(name, NULL);
}

void HogFramer::savePositive( const Mat& regionOnRoi, const Mat& regionOnBackground, int regId )
{
	//Scalar fm = cv::mean(regionOnRoi);
	//Scalar bm = cv::mean(regionOnBackground);

	//double m0 = (fm[0] + fm[1] + fm[2])/3;
	//double m1 = (bm[0] + bm[1] + bm[2])/3;

	unique_ptr<char> path;
	//if(m0 > m1)	//suspect a white car	
	//	path = nextWPath();		
	//else		//suspect a black car	
		path = nextBPath(regId);
	
	imwrite(path.get(), regionOnRoi);
}

unique_ptr<char> HogFramer::nextWPath(int regId)
{
	int count = ++pathCounters[regId][POSW_INDEX];
	char* name = new char[100];
	sprintf(name,"./../CarParkMonitor/Content/Images/posw-%d-%d.png", regId, count);

	return unique_ptr<char>(name);	
}

unique_ptr<char> HogFramer::nextBPath(int regId)
{	
	int count = ++pathCounters[regId][POSB_INDEX];
	char* name = new char[100];
	sprintf(name,"./../CarParkMonitor/Content/Images/posb-%d-%d.png",regId, count);

	return unique_ptr<char>(name);	
}

unique_ptr<char> HogFramer::nextNegPath(int regId)
{
	int count = ++pathCounters[regId][NEG_INDEX];
	char* name = new char[100];
	sprintf(name,"./../CarParkMonitor/Content/Images/neg-%d-%d.png", regId,count);

	return unique_ptr<char>(name);	
}

void HogFramer::initPathCounters( int len )
{
	pathCounters = new int*[len];
	for(int i = 0; i < len; i++)
	{
		pathCounters[i] = new int[3];
		for(int j = 0 ; j < 3; j++)
			pathCounters[i][j] = 0;
	}
}

