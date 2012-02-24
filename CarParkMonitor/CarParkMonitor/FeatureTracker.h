#pragma once
#include "FrameProcessor.h"

class FeatureTracker: public FrameProcessor
{
	public:	
		~FeatureTracker(void);

		cv::Mat currentGrayFrame;						// current gray-level image
		cv::Mat previousGrayFrame;						// previous gray-level image
		std::vector<cv::Point2f> points[2]; 			// tracked features from 0->1
		std::vector<cv::Point2f> initialFeaturePosition;  // initial position of tracked points
		std::vector<cv::Point2f> detectedFeatures;  	// detected features
		int maxDetectedFeatures;	  					// maximum number of features to detect
		double detectionQualityLevel;    				// quality level for feature detection
		double minFeatureDistance;   					// minimum distance between two feature points
		std::vector<uchar> featureStatuses; 			// status of tracked features
		std::vector<float> err;    						// error in tracking


		FeatureTracker(void){
			maxDetectedFeatures = 100;
			detectionQualityLevel = 0.070;
			minFeatureDistance = 30.;
		};

		void process(cv::Mat frame, cv::Mat* output){
		
			cv::cvtColor(frame, currentGrayFrame, CV_BGR2GRAY);
			frame.copyTo(*output);

			if(shouldAddNewPoints()){
				detectFeaturePoints();
				points[0].insert(points[0].end(),detectedFeatures.begin(),detectedFeatures.end());
				initialFeaturePosition.insert(initialFeaturePosition.end(),detectedFeatures.begin(),detectedFeatures.end());
			}
		
			if(previousGrayFrame.empty()){
				currentGrayFrame.copyTo(previousGrayFrame);
			}
		
			cv::calcOpticalFlowPyrLK(
				previousGrayFrame,
				currentGrayFrame, 	// 2 consecutive images
				points[0], 			// input point position in first image
				points[1], 			// output point postion in the second image
				featureStatuses,    // tracking success
				err);      			// tracking error
	
			rejectUntrackedFeatures();

			drawTrackedPoints(frame, *output);

			std::swap(points[1], points[0]);
			cv::swap(previousGrayFrame, currentGrayFrame);
		}

		void rejectUntrackedFeatures(){		
			int k = 0;
			for( int i = 0; i < points[1].size(); i++ ) {
				if (acceptTrackedPoint(i)) {			
					initialFeaturePosition[k]= initialFeaturePosition[i];
					points[1][k++] = points[1][i];
				}
			}

			points[1].resize(k);
			initialFeaturePosition.resize(k);
		}
	
		bool acceptTrackedPoint(int i) {
			return featureStatuses[i] &&
				// if point has moved
				(abs(points[0][i].x-points[1][i].x)+
				(abs(points[0][i].y-points[1][i].y))>2);
		}
	
		void drawTrackedPoints(cv:: Mat frame, cv:: Mat &output) {
			for(int i= 0; i < points[1].size(); i++ ) {
				// draw line and circle
				cv::line(output, initialFeaturePosition[i], points[1][i], cv::Scalar(255,255,255));
				cv::circle(output, points[1][i], 3, cv::Scalar(255,255,255),-1);
			}
		}

		bool shouldAddNewPoints(){
			return points[0].size() <= 10;
		}

		void detectFeaturePoints(){		
			cv::goodFeaturesToTrack(
				currentGrayFrame, 		   // the image 
				detectedFeatures,   	   // the output detected features
				maxDetectedFeatures,  	   // the maximum number of features 
				detectionQualityLevel,     // quality level
				minFeatureDistance);   	   // min distance between two features
		}
};