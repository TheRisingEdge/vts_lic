#include "HogClassifier.h"
#include "Tool.h"

static const int width = 64;//80;
static const int height = 64;

cv::Mat HogClassifier::readHPlane()
{
	auto f = FileStorage("./Content/Assets/hogsvm.yml", FileStorage::READ);	
	Mat hplane;
	f["hplane"] >> hplane;

	return hplane;
}

void HogClassifier::load()
{
	hogGpu = cv::gpu::HOGDescriptor(
		Size(width, height),//images size
		Size(16,16),//block size
		Size(8,8),//block stride
		Size(8,8),//cell size
		9
	);
	
	Mat hplane = readHPlane();
	hogGpu.setSVMDetector(hplane);	
}

vector<detection> HogClassifier::detect( ClassifierParams& params )
{
	auto blobs = params.blobs;
	Mat frame = params.frame;
	vector<Rect> rectDetections;
	vector<detection> detectionsToReturn;

	//Rect frameRect = Rect(0, 0, frame.cols, frame.rows);

	//Point extension = Point(width, height);
	int detectionId = 0;
	Mat fclone;
	cv::cvtColor(frame, fclone, CV_BGR2GRAY);
	gpuMat.upload(fclone);
	hogGpu.detectMultiScale(gpuMat,rectDetections);
	for_each(begin(rectDetections), end(rectDetections), [&](Rect r){
		detection d = {detectionId++, r}; 
		detectionsToReturn.push_back(d);
	});

	//for_each(begin(blobs), end(blobs), [&](const blob& b){
	//	Rect rect = b.rect;
	//	//Tool::extend(rect, extension);
	//	if(Tool::rectInside(rect, frameRect))
	//	{
	//		Mat region = fclone(rect);
	//		gpuMat.upload(region);			
	//		rectDetections.clear();
	//		hogGpu.detectMultiScale(gpuMat,rectDetections);
	//		
	//		for_each(begin(rectDetections), end(rectDetections), [&](Rect r){
	//			detection d = {0, r}; 
	//			detectionsToReturn.push_back(d);
	//		});
	//	}		
	//});

	fclone.release();
	//frame.release();
	return detectionsToReturn;
}


