#include "PSubtractor.h"

bool PSubtractor::openVideoCapture(int& fps, double& frameDelay)
{
   this->capture.open(this->videoPath);
   if (!capture.isOpened())
   {
      capture.release();
      return false;
   }

   fps        = capture.get(CV_CAP_PROP_FPS);
   frameDelay = 1000.0 / (double)(fps);
   return true;
}

void PSubtractor::run()
{
   int    fps        = 0;
   double frameDelay = 0;

   if (!openVideoCapture(fps, frameDelay))
      return;

   long frameCount = 0;
   Mat  frame;
   while (capture.read(frame))
   {
      SubFrame result = { frame };
      imshow("frame", frame);
      send(target, result);
      cv::waitKey(35);
      /*
       * if(frameCount < trainingFrames)
       * {
       * subtractor->learn(frame);
       * }else
       * {
       * Mat foregorund = subtractor->segment(frame);
       * SubFrame result = {frame, foregorund};
       * send(target, result);
       * cv::waitKey(5);
       * }
       *
       * frameCount++;*/
   }
}

