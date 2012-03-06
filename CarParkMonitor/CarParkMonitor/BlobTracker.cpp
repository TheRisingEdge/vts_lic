#include "BlobTracker.h"
#include <assert.h>
#include "BlobDetector.h"
#include "AppConfig.h"

BlobTracker::BlobTracker( TrackHistory* trackHistory )
{
	this->trackHistory = trackHistory;
	this->featureDetector = new cv::SiftFeatureDetector(0.001,0.005);	// new cv::GoodFeaturesToTrackDetector(
		//20, // maximum number of corners to be returned
		//0.01, // quality level
		//1); // minimum allowed distance between points;//
	this->descriptorExtractor = AppConfig::bowProperties.extractor;
}


BlobTracker::~BlobTracker(void)
{
}

vector<Point2f> getGoodFeatures(Mat image)
{
	vector<Point2f> keypoints;

	cv::goodFeaturesToTrack(
		image, 		   // the image 
		keypoints,   	   // the output detected features
		20,  	   // the maximum number of features 
		0.001,     // quality level
		0.001);   	   // min distance between two features

	return keypoints;
}

vector<Point2f> getGoodFeatures(Mat image, Mat mask)
{
	vector<Point2f> keypoints;

	cv::goodFeaturesToTrack(
		image, 		   // the image 
		keypoints,   	   // the output detected features
		10,  	   // the maximum number of features 
		0.001,     // quality level
		0.001,
		mask
	);   	   // min distance between two features

	return keypoints;
}

void drawTrackedPoints(vector<Point2f> pts,cv:: Mat &output) {

	int size = pts.size();
	for(int i= 0; i < size; i++ ) {
		// draw line and circle		
		cv::circle(output, pts[i], 3, cv::Scalar(255,255,255),-1);
	}
}

void drawTrackedPoints(vector<Point2f> prev, vector<Point2f> curr, cv:: Mat &output) {
	int size = prev.size();
	for(int i= 0; i < size; i++ ) {
		// draw line and circle
		//cv::line(output, prev[i], curr[i], cv::Scalar(255,255,255));
		cv::circle(output, curr[i], 2, cv::Scalar(255,255,255),-1);
	}
}

void drawBlobs(vector<blob*> blobs, Mat &output)
{
	int size = blobs.size();
	Scalar color = Scalar( 70, 255, 100 );
	Scalar red = Scalar( 0, 0, 255 );

	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int thickness = 1;  

	for( int i = 0; i< size; i++ )
	{
		Rect rect = blobs.at(i)->sourceRect;

		rectangle( output, rect.tl(), rect.br(), color);

		stringstream id;//create a stringstream
		id << blobs.at(i)->id;//add number to the stream

		string text = id.str();
				
		cv::Point textOrg = blobs.at(i)->sourceRect.tl();
		cv::putText(output, text, textOrg, fontFace, fontScale, red, thickness,8);
	}
}

//void rejectUntrackedFeatures(vector<Point2f>* points, vector<uchar> statuses){		
//	int k = 0;
//	int size = points->size();
//	for( int i = 0; size; i++ ) {
//		if (acceptTrackedPoint(i)) {			
//			initialFeaturePosition[k]= initialFeaturePosition[i];
//			points[1][k++] = points[1][i];
//		}
//	}
//
//	points[1].resize(k);
//	initialFeaturePosition.resize(k);
//}
//
//bool acceptTrackedPoint(int i) {
//	return featureStatuses[i] &&
//		// if point has moved
//		(abs(points[0][i].x-points[1][i].x)+
//		(abs(points[0][i].y-points[1][i].y))>2);
//}

int containedPointsInRect(Rect rect, vector<Point2f> pts)
{
	vector<Point2f>::iterator it = pts.begin();
	vector<Point2f>::iterator end = pts.end();
	
	int contained = 0;

	for(;it!=end; it++)
	{
		if(rect.contains(*it))
		{
			contained++;
		}
	}

	return contained;
}

void BlobTracker::match( MatcherParams param, MatcherResult* result )
{
	int frameCount 				= param.frameCount;
	vector<blob*> detectedBlobs = param.detectedBlobs;
	Mat frame 					= param.frame;
	Mat foreground				= param.foreground;
	Mat prevFrame 				= param.previousFrame;
	Mat prevForeground 			= param.previousForeground;
	vector<blob*> prevBlobs 	= trackHistory->previousBlobs;


	vector<Point2f> prevFrameKeypoints;
	vector<Point2f> currentFrameKeypoints;
	vector<Point2f> correctTracked;
	vector<uchar> trackingStatus;
	vector<float> trackingErrors;

	Mat grayFrame 	  = param.grayFrame;
	Mat prevGrayFrame = param.previousGrayFrame;
	
	if(prevBlobs.size() == 0 && detectedBlobs.size() == 0)	
	{
		return;
	}

	if(prevBlobs.size() == 0 && detectedBlobs.size() != 0)
	{
		result->newBlobs.assign(detectedBlobs.begin(), detectedBlobs.end());
		return;
	}

	int size = prevBlobs.size();
	for(int i = 0; i < size; i++)
	{
		blob* b = prevBlobs[i];

		Rect blobRect = b->sourceRect;		 
		Mat blobImage = prevGrayFrame(blobRect);
		Mat blobForeground = prevForeground(blobRect);

		vector<Point2f> keypoints = getGoodFeatures(blobImage, blobForeground);
		//b->descriptor.pointFeatures = keypoints;

		assert(b->hash.size() == 0);
		Point2f tl = blobRect.tl();
		vector<Point2f>::iterator it = keypoints.begin();
		for(; it != keypoints.end() ; it++)
		{		
			b->temp.trackIndexes.push_back(prevFrameKeypoints.size());			
			prevFrameKeypoints.push_back(tl + (*it));			
		}		
	}
	
	if(prevFrameKeypoints.size() == 0)
		return;

	cv::calcOpticalFlowPyrLK(
		prevGrayFrame,			
		grayFrame, 				// 2 consecutive images, must be gray
		prevFrameKeypoints, 	// input point position in first image
		currentFrameKeypoints, 	// output point postion in the second image
		trackingStatus,			// tracking success
		trackingErrors			// tracking error  	   
	);  	

	//remap points to blobs
	for(int i = 0; i < prevBlobs.size(); i++)
	{
		blob* b = prevBlobs[i];
		int size = b->temp.trackIndexes.size();
		for(int j = 0; j < size; j++)
		{
			b->temp.trackedPoints.push_back(currentFrameKeypoints[b->temp.trackIndexes[j]]);
		}
	}

	if(frameCount == 1140)
	{
		int a;
 		a = 3;
	}
	


#pragma region matching

	int prevBlobsSize = prevBlobs.size();
	for(int j = 0; j < prevBlobsSize; j++)
	{
		blob* prevBlob = prevBlobs[j];		
		int trackedPointsSize = prevBlob->temp.trackedPoints.size();


		float confidence = 0;
		float trackedIndex = -1;

		for(int i = 0; i < detectedBlobs.size(); i++)
		{
			blob* detectedBlob = detectedBlobs[i];

			int inliers = 0;			
			vector<Point2f>::iterator it = prevBlob->temp.trackedPoints.begin();
			vector<Point2f>::iterator end = prevBlob->temp.trackedPoints.end();

			for(; it != end ; it++)
			{
				if( (*it).inside(detectedBlob->sourceRect))
				{
					inliers++;
				}
			}

			float conf = ((float)inliers)/((float)trackedPointsSize);
			if(conf > 0.5 && conf > confidence)
			{
				confidence = conf;
				trackedIndex = i;
			}
		}

		if(trackedIndex != -1)
		{			
			blob* matched = detectedBlobs[trackedIndex];
			if(matched->temp.prevConfidence < confidence )
			{
				matched->id = prevBlob->id;
				matched->temp.trackedByPrev = true;
				matched->temp.prevConfidence = confidence;
			}			
		}else
		{
			prevBlob->temp.wasLost = true;
		}
	}

	  
	for(int i = 0; i < detectedBlobs.size(); i++)
	{
		blob* b = detectedBlobs[i];
		if(b->temp.trackedByPrev == false)
		{//check lost blobs
			injectBlobDescription(b, frame, foreground);
			vector<TrackInfo*> lostTracks = this->trackHistory->getLost();
			for(int k = 0; k < lostTracks.size(); k++)
			{
				TrackInfo* track = lostTracks[k];
				for(int s = 0 ; s < track->sifts.size(); s++)
				{
					Mat sift = track->sifts[s];
					vector<KeyPoint> kps = track->keypoints[s];
					Mat img = track->images[s];

					//-- Step 3: Matching descriptor vectors with a brute force matcher
					BruteForceMatcher< L2<float> > matcher;
					std::vector< DMatch > matches;
					matcher.match( b->descriptor.descriptor, sift, matches );
					

					//-- Draw matches
					Mat img_matches;
					drawMatches( frame(b->sourceRect), b->descriptor.keypoints,
								 img, kps, matches, img_matches );

					imshow("matches", img_matches);
					
					waitKey();
					img_matches.release();
				}
			}

		//}else//realy a new blob
		//{
			this->injectBlobDescription(b, frame, foreground);
			result->newBlobs.push_back(b);
		}
	}

	for(int i = 0; i < prevBlobs.size(); i++)
	{
		blob* b = prevBlobs[i];
		if(b->temp.wasLost)
		{
			this->injectBlobDescription(b, prevFrame, prevForeground);
			result->prevLostBlobs.push_back(b);			
		}
	}

#pragma endregion matching

	Mat cl = frame.clone();

	cout << param.frameCount << "\n";
	drawTrackedPoints(prevFrameKeypoints, currentFrameKeypoints, cl);	
	drawBlobs(detectedBlobs, cl);

	imshow("blobed", cl);

	cl.release();
	//for each detected blob filter keypoints with respect to its rect (it is a keypoint filter class)
	//map the keypoints back to a previous blob
	//find simple blob match

	//debug draw
}

void BlobTracker::injectBlobDescription( blob* b, Mat image, Mat foreground )
{
	Mat subimage = image(b->sourceRect);	
	vector<KeyPoint> keypoints;
	
	Mat descriptors;	
	Mat mask = foreground;
	CV_Assert( mask.empty() || (mask.type() == CV_8UC1 && mask.size() == image.size()) );
	this->featureDetector->detect(subimage, keypoints, Mat());
	this->descriptorExtractor->compute(subimage, keypoints, descriptors);
	
	Mat img_keypoint;
	cv::drawKeypoints( subimage, keypoints, img_keypoint, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	imshow("bl", subimage);
	imshow("kps", img_keypoint);

	//cv::waitKey();

	b->descriptor.descriptor = descriptors;
	b->descriptor.keypoints = keypoints;
	b->descriptor.image = image(b->sourceRect).clone();
}
