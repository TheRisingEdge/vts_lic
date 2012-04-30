#include "LbpMatcher.h"
#include "histogram.h"
#include "lbp.h"

cv::Mat LbpMatcher::lbpHist( Mat& src )
{
	auto processed = lbp::ELBP(src,8,8);
	auto hist = lbp::spatial_histogram(processed, 256, 3, 3);
	return hist;
}

float LbpMatcher::match( track& tr, detection& dt, Mat& frame )
{
	if(detectionHists.find(dt.id) == detectionHists.end())
	{
		auto detHist = lbpHist(frame(dt.rect));
		detectionHists[dt.id] = detHist;
	}

	auto dhist = detectionHists[dt.id];
	double dist = lbp::chi_square(tr.model.lbpHist, dhist);

	return (float)dist;
}

void LbpMatcher::inferModel( track& tr, detection& dt, Mat& frame )
{
	if(detectionHists.find(dt.id) == detectionHists.end())
	{
		detectionHists[dt.id] = lbpHist(frame(dt.rect));		
	}
	
	tr.model.lbpHist = detectionHists[dt.id];		
}

float LbpMatcher::distance( track& tr, Mat& src )
{
	Mat desc;
	return distance(tr, src, desc);
}

float LbpMatcher::distance( track& tr, Mat& src, Mat& descriptor )
{
	descriptor = lbpHist(src);
	double lkDist = lbp::chi_square(tr.model.lbpHist, descriptor);
	return lkDist;	
}