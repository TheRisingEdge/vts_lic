#include "AutoTracker.h"
#include "Blob.h"
#include "Helper.h"
#include <algorithm>
#include "EstimatorCollection.h"
#include <agents.h>
#include "MogSubtractor.h"
#include "PSubtractor.h"
#include "PDetector.h"
#include "AvgSubtractor.h"
#include "StableAvgSubtractor.h"
#include "PClassifier.h"
#include "HogClassifier.h"
#include "Video.h"
#include "PTracker.h"
#include "Matcher2D.h"

using namespace Concurrency;

using namespace std;

AutoTracker::AutoTracker( AutoTrackerParam param )
{
	this->videoPath = param.videoPath;
	this->foregroungSegmentator = param.foregroundSegmentator;
	this->blobDetector = param.blobDetector;

	assert(this->videoPath);
	assert(this->foregroungSegmentator);
	assert(this->blobDetector);
}

AutoTracker::~AutoTracker()
{

}

bool AutoTracker::openVideoCapture(int& fps, double& frameDelay)
{
	this->capture.open(this->videoPath);

	if(!capture.isOpened())
	{
		capture.release();
		return false;
	}
	
	fps = capture.get(CV_CAP_PROP_FPS);		
	frameDelay = 1000.0/(double)(fps);

	return true;
}

void AutoTracker::run()
{
	BgSubtractorBase* subtractor = new MogSubtractor("mog");//new StableAvgSubtractor();	
	BlobDetector* detector = new BlobDetector(15,20,"detector");
	ClassifierBase* classifier = new HogClassifier();

	//unbounded_buffer<SubFrame> subtractorOut;	
	//PSubtractor psubtractor = PSubtractor(subtractor, subtractorOut, videoPath, 100);
	//
	//unbounded_buffer<DetectionFrame> detectorOut;
	//
	//PDetector pdetector = PDetector(detector, subtractorOut, detectorOut);
	//unbounded_buffer<ClasifierFrame> classifierOut;
	//
	//PClassifier pclassifier = PClassifier(classifier, detectorOut, classifierOut);

	//psubtractor.start();
	//pdetector.start();
	//pclassifier.start();

	//agent::wait(&psubtractor);
	//agent::wait(&pdetector);
	//agent::wait(&pclassifier);

	//subtractor = new AvgSubtractor();
	Video vid = Video(videoPath);
	PTracker tracker = PTracker(vid, subtractor, classifier, new Matcher2D());
	tracker.start();
}
