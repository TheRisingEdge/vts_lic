#pragma once
#include "BgSubtractorBase.h"
#include <agents.h>
#include <iostream>

using namespace Concurrency;
using namespace std;

struct SubFrame
{
   Mat frame;
   Mat foreground;
};

class PSubtractor : public agent
{
private:
   BgSubtractorBase   *subtractor;
   ITarget<SubFrame>& target;
   int                trainingFrames;

   char   *videoPath;
   VideoCapture capture;
   bool openVideoCapture(int& fps, double& frameDelay);

public:
   PSubtractor(BgSubtractorBase *subtractor, ITarget<SubFrame>& tbuffer, char *videoPath, int trainingFrames) :
      target(tbuffer)
   {
      this->subtractor     = subtractor;
      this->target         = target;
      this->videoPath      = videoPath;
      this->trainingFrames = trainingFrames;
   }

   ~PSubtractor() {}

protected:
   void run();
};
