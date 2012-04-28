#pragma once
#include "TrackMatcher.h"
#include "PTracker.h"

class Matcher2D: public TrackMatcher
{
public:
	Matcher2D(){}
	~Matcher2D(){}

	float match(track& tr, detection& dt);
	void inferModel(track& tr, detection& dt);
};