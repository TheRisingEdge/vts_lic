#include "PDetector.h"
#include "Draw.h"

void PDetector::run()
{
   while (true)
   {	
		Mat frame = receive(frameBuffer);

      //SubFrame sframe = receive(source);

      //imshow("frame", sframe.frame);
      //imshow("foreground", sframe.foreground);

      //DetectionFrame dframe = { sframe.frame };
      //send(targetBuffer, dframe);

/*		DetectorParams params;
 *              params.frame = sframe.frame;
 *              params.foreground = sframe.foreground;
 *
 *              auto detections = detector->detect(params);
 *              if(detections.size() > 0)
 *              {
 *                      Mat fclone = sframe.frame.clone();
 *                      for(int i = 0 ; i< detections.size(); i++)
 *                      {
 *                              Draw::rect(detections[i].rect, fclone);
 *                      }
 *                      fclone.release();
 *
 *
 *                      DetectionFrame dframe = {sframe.frame, sframe.foreground, detections};
 *                      send(target, dframe);
 *                      cv::waitKey(5);
 *              }	*/
   }
}
