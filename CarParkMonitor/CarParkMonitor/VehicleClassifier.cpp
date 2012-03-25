#include "VehicleClassifier.h"


VehicleClassifier::VehicleClassifier(void)
{
}


VehicleClassifier::~VehicleClassifier(void)
{
}

vector<shared_ptr<carDetection>> VehicleClassifier::detect( const ClassifierParam& param )
{
	Mat frame = param.frame;
	Mat foreground = param.foreground;
	auto blobs = param.blobBuffer;

	vector<shared_ptr<carDetection>> result;
	for_each(begin(blobs), end(blobs), [&result](shared_ptr<blob> b){

		auto detection = new carDetection;
		detection->id = ID_UNDEFINED;
		detection->blobId = b->id;
		detection->rect = b->rect;
		auto vehicle = shared_ptr<carDetection>(detection);
		result.push_back(vehicle);
	});

	return result;
}