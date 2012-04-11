#include "opencv2\opencv.hpp"
#include "BlobTracker.h"
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
   BlobTracker      *blobTracker;
   TrackHistory     *trackHistory;
};

class AutoTracker {
private:
   char             *videoPath;
   BgSubtractorBase *foregroungSegmentator;
   BlobDetector     *blobDetector;
   BlobTracker      *blobTracker;
   TrackHistory     *trackHistory;

   TrackerParam trackerParam;

   VideoCapture capture;
   bool openVideoCapture(int& fps, double& frameDelay);

public:
	~AutoTracker();
   AutoTracker(AutoTrackerParam param);
   
   void run();
};
