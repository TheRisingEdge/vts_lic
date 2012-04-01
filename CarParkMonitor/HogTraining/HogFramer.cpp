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

using namespace Concurrency;

DrawingControl HogFramer::drawingControl;
vector<Rect> HogFramer::monitoredRegions;

const char* frameWindowName = "frame";
static AvgSubtractor* avg;
static MogSubtractor* mog;

static const int width = 80;
static const int height = 64;

HogFramer::HogFramer( HogFramerParams params ):FrameProcessor("Hog Extractor")
{
	this->params = params;
	this->blobDetector = new BlobDetector(30, 30, "detector");

	HogFramer::drawingControl.drawing = false;

	frameCount = 0;
	trainingFrames = 100;
	posCount = 0;
	negCount = 0;			 
}


HogFramer::~HogFramer(void)
{
}
static const double pi = 3.1416;	
bool removeBlob(shared_ptr<blob> b)
{
	Mat hull;
	cv::convexHull(b->contour, hull);
	double area = cv::contourArea(hull);
	double perimeter = cv::arcLength(hull, true);
	double th = (4*pi*area)/(perimeter*perimeter);
	if(th < 0.6)
		return true;

	return false;		
}

void HogFramer::process( const Mat& frame )
{
	Mat frameClone = frame.clone();	

	frameCount++;
	if(frameCount == 4)
	{
		selectMonitorRegions(frame);		
		for(int i = 0; i < monitoredRegions.size(); i++)
		{
			RegionExtractor ext;
			ext.rect = monitoredRegions[i];
			ext.bgSubtractor = new MogSubtractor();

			char* windowName = new char[50];
			sprintf(windowName, "roi-%d", i);
			ext.windowName = windowName;

			regionExtractors.push_back(ext);
		}

	}else
	{
		for_each(regionExtractors.begin(), regionExtractors.end(), [&](const RegionExtractor& ext)
		{			
			Draw::rect(ext.rect, frameClone);

			Mat roi = frame(ext.rect);
			imshow(ext.windowName, roi);

			if(frameCount < trainingFrames)
			{
				ext.bgSubtractor->learn(roi);
			}else
			{
				Mat foreground = ext.bgSubtractor->segment(roi);

				DetectorParams dparam;
				dparam.foreground = foreground;
				dparam.frame = roi;
				vector<shared_ptr<blob>> blobs = blobDetector->detect(dparam);
				
				std::remove_if(blobs.begin(), blobs.end(),removeBlob);
				savePositiveAndNegatives(blobs,ext.rect, frame, foreground);
			}
		});
	}

	imshow(frameWindowName, frameClone);
	frameClone.release();
}

void HogFramer::savePositiveAndNegatives( vector<shared_ptr<blob>> blobs,const Rect& crop, const Mat& frame, const Mat& mask)
{
	for_each(begin(blobs), end(blobs), [&](shared_ptr<blob> b)
	{		
		Point cropTopLeft = crop.tl();

		Rect blobRect = b->rect;
		Point blobTopLeft = blobRect.tl();
		Point blobCenter = cropTopLeft + blobTopLeft + Point(blobRect.width/2, blobRect.height/2);
		
		Rect r(0,0,width,height);
		Tool::rectToCenter(blobCenter, r);

		Mat region = frame(r);
		char* path = nextPositivePath();

		imshow("saving",region);
		imwrite(path, region);
		
		//generating negative samples centered in the corners of the car detection
		generateNegativeSamples(r, frame);		
		delete[] path;
	});
}

char* HogFramer::nextPositivePath( )
{
	posCount++;
	char* name = new char[50];
	sprintf(name,"./../CarParkMonitor/Content/Images/pos-%d.png", posCount);
	return name;
}

char* HogFramer::nextNegativePath()
{
	negCount++;
	char* name = new char[50];
	sprintf(name,"../CarParkMonitor/Content/Images/neg-%d.png", negCount);
	return name;
}

void HogFramer::generateNegativeSamples( const Rect& r, const Mat& frame )
{
	static Rect frameRect(0,0, frame.cols, frame.rows);

	Rect topLeft = Rect(0,0,width, height);
	Tool::rectToCenter(r.tl(), topLeft);

	Rect topRight(0,0, width, height);
	Point p = r.tl() + Point(width, 0);
	Tool::rectToCenter(p, topRight);
	

	Rect bottomLeft = Rect(0,0, width, height);
	p = r.tl() - Point(0, height);
	Tool::rectToCenter(p, bottomLeft);

	Rect bottomRight = Rect(0,0, width, height);
	p = r.br();
	Tool::rectToCenter(p, bottomRight);

	char* path;
	if(Tool::rectInside(topLeft,frameRect))
	{
		path = nextNegativePath();
		imwrite(path, frame(topLeft));
		delete[] path;
	}
	
	if(Tool::rectInside(topRight,frameRect))
	{
		path = nextNegativePath();
		imwrite(path, frame(topRight));
		delete[] path;
	}
	
	if(Tool::rectInside(bottomLeft, frameRect))
	{
		path = nextNegativePath();
		imwrite(path, frame(bottomLeft));
		delete[] path;
	}
	
	if(Tool::rectInside(bottomRight, frameRect))
	{
		path = nextNegativePath();
		imwrite(path, frame(bottomRight));
		delete[] path;
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
			int h = height+10;//abs(HogFramer::drawingControl.y - y);
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

