#pragma once
#include "ClassifierBase.h"

struct model
{	
	Rect kalmanRect;
};

struct track
{
	int id;
	Rect_<float> rectf;	
	model model;

	void assign(Rect rect)
	{
		Point2f tl = rect.tl();
		rectf = Rect_<float>(tl.x, tl.y, rect.width, rect.height);
	}

	Rect asRecti()
	{
		return Rect(rectf.x, rectf.y, rectf.width, rectf.height);
	}
};

class TrackMatcher
{
public:
	TrackMatcher(){};
	~TrackMatcher(){};
	virtual float match(track& tr, detection& dt) = 0;
	virtual void inferModel(track& tr, detection& dt) = 0;
};