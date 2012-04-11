#pragma once
#include <agents.h>
#include "PDetector.h"
#include "ClassifierBase.h"

using namespace Concurrency;

struct ClasifierFrame {};

class PClassifier : public agent
{
	ClassifierBase *classifier;
	ISource<DetectionFrame>& source;
	ITarget<ClasifierFrame>& target;
	
	char* videoPath;
	VideoCapture capture;		
	bool openVideoCapture(int& fps, double& frameDelay);

public:	
	void setVideo(char *videoPath);
	PClassifier(ClassifierBase *cls, ISource<DetectionFrame>& src, ITarget<ClasifierFrame>& dst) :
		source(src),
		target(dst),
		classifier(cls){}

	~PClassifier(){}

protected:
	void run();
};
