#pragma once
#include <agents.h>
#include "BlobDetector.h"
#include "PSubtractor.h"

using namespace Concurrency;

struct DetectionFrame
{
   Mat          frame;
   Mat          foregorund;
   vector<blob> blobs;
};

class PDetector : public agent
{
private:   
   ISource<Mat>& frameBuffer;
   ITarget<DetectionFrame>& targetBuffer;
   BlobDetector *detector;

public:
   PDetector(BlobDetector *blobDetector, ISource<Mat>& frameBuff, ITarget<DetectionFrame>& targetBuff) 
	   :frameBuffer(frameBuff),
		targetBuffer(targetBuff),
		detector(blobDetector)
	{}

   ~PDetector() {}

protected:
   void run();
};
