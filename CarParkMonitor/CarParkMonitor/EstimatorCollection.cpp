#include "EstimatorCollection.h"


EstimatorCollection::EstimatorCollection(void)
{
}

EstimatorCollection::~EstimatorCollection(void)
{
}

shared_ptr<KalmanEstimator> EstimatorCollection::add( shared_ptr<carDetection> b )
{
	int id = b->id;
	auto it = estimatorsMap.find(id);

	shared_ptr<KalmanEstimator> filter = nullptr;
	if(it == estimatorsMap.end())
	{
		filter = shared_ptr<KalmanEstimator>(new KalmanEstimator);
		filter->init(b);
		estimatorsMap[id] = filter;
	}else
	{
		filter = estimatorsMap[id];
	}

	return filter;
}

void EstimatorCollection::remove( int id )
{
	auto it = estimatorsMap.find(id);
	if(it == estimatorsMap.end())
		return;

	estimatorsMap.erase(id);
}

bool EstimatorCollection::contains( int id )
{
	auto it = estimatorsMap.find(id);
	return it != estimatorsMap.end();
}

void EstimatorCollection::update( shared_ptr<carDetection> b )
{
	auto it = estimatorsMap.find(b->id);
	if(it == estimatorsMap.end())
	{
		add(b);
		return;
	}

	auto filter = (*it).second;
	auto result = filter->update(b);
}

shared_ptr<KalmanEstimator> EstimatorCollection::get( int id )
{
	auto it = estimatorsMap.find(id);	
	if(it == estimatorsMap.end())
		return nullptr;

	return (*it).second;	
}

