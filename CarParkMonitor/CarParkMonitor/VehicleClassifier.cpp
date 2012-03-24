#include "VehicleClassifier.h"


VehicleClassifier::VehicleClassifier(void)
{
}


VehicleClassifier::~VehicleClassifier(void)
{
}

void VehicleClassifier::detect( const Mat& image, vector<Rect>& detections )
{

}

void VehicleClassifier::detect( const ClassifierParam& param, ClassifierResult& result )
{
	Mat frame = param.frame;
	auto blobs = param.blobBuffer;

	for_each(begin(blobs), end(blobs), [&result](shared_ptr<blob> b){

		auto detection = new vehicleDetection;
		detection->blobId = b->id;
		detection->rect = b->rect;
		auto vehicle = shared_ptr<vehicleDetection>(detection);
		result.detections.push_back(vehicle);
	});
}

float VehicleClassifier::detectVehicle( const Mat& image,const Mat& mask )
{
	return 0.0;
}
