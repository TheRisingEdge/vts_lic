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

static const int KEEP = 0;
static const int DISCARD = 1;

vector<detection> HogClassifier::detect( ClassifierParams& params )
{
	Mat frame = params.frame;
	vector<Rect> rectDetections;
	vector<detection> detectionsToReturn;

	int detectionId = 0;
	Mat grayClone;
	cv::cvtColor(frame, grayClone, CV_BGR2GRAY);
	gpuMat.upload(grayClone);
	hogGpu.detectMultiScale(gpuMat,rectDetections);
	gpuMat.release();
	grayClone.release();

	int size = rectDetections.size();
	int* status = new int[size]; 	
	memset(status, KEEP, size*sizeof(int));

	for(int i = 0; i < size - 1; i++)
	{		
		for(int j = i+1; j < size; j++)
		{
			if(status[i] == KEEP && status[j] == KEEP)
			{
				int min;
				if(rectDetections[i].area() < rectDetections[j].area())
					min = i;
				else
					min = j;

				Rect intersection = rectDetections[i] & rectDetections[j];
				if(intersection.area() > 0.4*rectDetections[min].area())
				{
					status[min] = DISCARD;
				}
			}			
		}
	}

	for(int i = 0; i < size; i++)	
		if(status[i] == KEEP)
		{
			detection det = {detectionId++, rectDetections[i]}; 
			detectionsToReturn.push_back(det);
		}
	
	delete[] status;	
	
	return detectionsToReturn;
}


