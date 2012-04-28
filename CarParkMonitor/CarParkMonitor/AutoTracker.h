#include "opencv2\opencv.hpp"
#include "BlobDetector.h"
#include "BgSubtractorBase.h"

using namespace cv;

#define FRAME_DRAW    1

struct AutoTrackerParam
{
   char             *videoPath;
   int              trainingFrames;

   BgSubtractorBase *foregroundSegmentator;
   BlobDetector     *blobDetector;
};

class AutoTracker {
private:
   char             *videoPath;
   BgSubtractorBase *foregroungSegmentator;
   BlobDetector     *blobDetector;

   VideoCapture capture;
   bool openVideoCapture(int& fps, double& frameDelay);

public:
	~AutoTracker();
   AutoTracker(AutoTrackerParam param);
   
   void run();
};
