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
   ISource<SubFrame>&       source;
   ITarget<DetectionFrame>& target;

   BlobDetector *detector;

public:
   PDetector(BlobDetector *bdetector, ISource<SubFrame>& src, ITarget<DetectionFrame>& trg) :
      source(src),
      target(trg)
   {
      this->detector = bdetector;
   }

   ~PDetector() {}

protected:
   void run();
};
