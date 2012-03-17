#include "TrackHistory.h"

TrackHistory::TrackHistory( int size )
{
	this->size = 100;	
}

void TrackHistory::append( blob b )
{

}

void TrackHistory::update(TrackParam* param)
{
	vector<blob*> tracked = param->trackedBlobs;

 	vector<blob*>::iterator it = tracked.begin();
	vector<blob*>::iterator end = tracked.end();	

 	for(; it != end; it++)
	{		
		blob* b = *it;
		int id = b->id;

		map<int, TrackInfo*>::iterator t = infos.find(id);
		if(t != infos.end())//found history for this blob
		{
   			TrackInfo* info = t->second;
			info->isLost = false;
			info->trackedPoints.insert(info->trackedPoints.begin(), b->rect.tl());
			
			if(info->trackedPoints.size() > this->size)
      			info->trackedPoints.resize(this->size);

		}else // history not found for this blob
		{
			TrackInfo* info = new TrackInfo;
			info->id = id;
			info->isLost = false;
			//make snapshot		

			info->trackedPoints.insert(info->trackedPoints.begin(), b->rect.tl());

			if(info->trackedPoints.size() > this->size)
				info->trackedPoints.resize(this->size);

			infos[id] = info;
		}
	}

	
	it = param->prevLostBlobs.begin();
	end = param->prevLostBlobs.end();

	for(; it != end; it++)
	{		
		blob* b = *it;
		int id = b->id;

		map<int, TrackInfo*>::iterator t = infos.find(id);
		if(t != infos.end())//found history for this blob
		{
			TrackInfo* info = t->second;
			info->isLost = true;

			//assert(b->descriptor.descriptor.empty() == false);

			//info->sifts.push_back(b->descriptor.descriptor);
			//info->keypoints.push_back(b->descriptor.keypoints);
			//info->images.push_back(b->descriptor.image);

		}else
		{
			//this should not happen
			assert(false);
		}
	}
}

vector<TrackInfo*> TrackHistory::getLost()
{
	
	map<int,TrackInfo*>::iterator it = infos.begin();
	vector<TrackInfo*> ll;
	
	for (; it != infos.end(); it++ )
	{	
		if((*it).second->isLost)
			ll.push_back((*it).second);
	}
		
	return ll;
}



