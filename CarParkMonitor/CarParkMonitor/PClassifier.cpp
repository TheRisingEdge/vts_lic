#include "PClassifier.h"
#include "ClassifierBase.h"
#include "Draw.h"

bool PClassifier::openVideoCapture(int& fps, double& frameDelay)
{
	this->capture.open(videoPath);
	if (!capture.isOpened())
	{
		capture.release();
		return false;
	}

	fps        = capture.get(CV_CAP_PROP_FPS);
	frameDelay = 1000.0 / (double)(fps);
	return true;
}

void PClassifier::setVideo( char *videoPath )
{
	this->videoPath = videoPath;
}

void PClassifier::run()
{
   vector<detection> detections;
   while (true)
   {
      auto detFrame = receive(source);
      auto blobs = detFrame.blobs;
      Mat  frame = detFrame.frame;

      detections.clear();
      ClassifierParams param = { frame, blobs };
      auto detections  = classifier->detect(param);

      if (detections.size() > 0)
      {
         auto fclone = frame.clone();
         for_each(begin(detections), end(detections), [&](const detection &d) {
            Draw::rect(d.rect, fclone);
         });

         imshow("classifier", fclone);
         fclone.release();
      }
      cv::waitKey(5);
   }
}