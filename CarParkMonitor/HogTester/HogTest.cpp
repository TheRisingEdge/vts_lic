#include "HogTest.h"
#include "HogTest.h"
#include "Draw.h"

static const int width = 80;
static const int height = 64;
static const int fvectorSize = 2268;

HogTest::HogTest():FrameProcessor("hog-test")
{	
	hog = new cv::HOGDescriptor(
		Size(width, height),//images size
		Size(16,16),//block size
		Size(8,8),//block stride
		Size(8,8),//cell size
		9
	);

	hog_gpu = new cv::gpu::HOGDescriptor(
		Size(width, height),//images size
		Size(16,16),//block size
		Size(8,8),//block stride
		Size(8,8),//cell size
		9
	);

	auto f = FileStorage("./../CarParkMonitor/Content/Assets/hogsvm.yml", FileStorage::READ);
	Mat hplane;
	f["hplane"] >> hplane;
	hog->setSVMDetector(hplane);

	hog_gpu->setSVMDetector(hplane);	
}

void HogTest::process( const Mat& frame )
{
	auto fclone = frame.clone();
	cv::cvtColor(fclone, fclone, CV_BGR2GRAY);

	GpuMat gframe;
	gframe.upload(fclone);

	vector<Rect> found;
	hog_gpu->detectMultiScale(gframe, found);
	
	for(int i = 0; i < found.size(); i++ )
	{
		Rect r = found[i];
		Draw::rect(r, fclone);	
	}

	imshow("detections", fclone);
	gframe.release();
	fclone.release();

}


