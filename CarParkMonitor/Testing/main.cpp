/* Kinetic Adaptive Regional Object Localizer by Arthur Amarra
 * implementation of "Median flow" tracker (Kalal, et. al)
 */

#include <stdio.h>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

CvRect box;
bool drawing_box = false;
bool drawn_box = false;

inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
	if ( *img != NULL )	{ //if image already exists
		return;
	}
	else {
		*img = cvCreateImage( size, depth, channels );
	}
	if ( *img == NULL )
	{
		fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");
		exit(-1);
	}
}

inline double euclid_dist(const CvPoint2D32f* point1, const CvPoint2D32f* point2) {
	//this function calculates the euclidean distance between 2 points
	double distance, xvec, yvec;
	xvec = point2->x - point1->x;
	yvec = point2->y - point1->y;
	distance = sqrt((xvec * xvec) + (yvec * yvec));
	return distance;
}

void pairwise_dist(const CvPoint2D32f* features, double *edist, int npoin) {
	//calculate m x n euclidean pairwise distance matrix.
	for (int i = 0; i < npoin; i++) {
		for (int j = 0; j < npoin; j++) {
			int ind = npoin*i + j;
			edist[ind] = euclid_dist(&features[i],&features[j]);
		}
	}
}

void draw_box(IplImage *temp, CvRect box) {
	cvRectangle( temp, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),
						cvScalar(0xff,0x00,0x00) );
}

void my_mouse_callback( int event, int x, int y, int flags, void* param ){
	IplImage* image = (IplImage*) param;

	switch( event ){
		case CV_EVENT_MOUSEMOVE: 
			if( drawing_box ){
				box.width = x - box.x;
				box.height = y - box.y;
			}
			break;

		case CV_EVENT_LBUTTONDOWN:
			drawing_box = true;
			box = cvRect( x, y, 0, 0 );
			break;

		case CV_EVENT_LBUTTONUP:
			drawing_box = false;
			drawn_box = true;
			if( box.width < 0 ){
				box.x += box.width;
				box.width *= -1;
			}
			if( box.height < 0 ){
				box.y += box.height;
				box.height *= -1;
			}
			draw_box( image, box );
			break;
	}
}

void ncc_filter(IplImage *frame1, IplImage *frame2, CvPoint2D32f *prev_feat, CvPoint2D32f *curr_feat, 
				int npoin, int method, IplImage *rec0, IplImage *rec1, IplImage *res, int *ncc_pass) {
	int filt = npoin/2;
	vector<float> ncc_err (npoin,0.0);

	for (int i = 0; i < npoin; i++) {
		cvGetRectSubPix( frame1, rec0, prev_feat[i] );
		cvGetRectSubPix( frame2, rec1, curr_feat[i] );
		cvMatchTemplate( rec0,rec1, res, method );
		ncc_err[i] = ((float *)(res->imageData))[0]; 
	}
	vector<float> err_copy (ncc_err);
	sort(ncc_err.begin(), ncc_err.end());
	double median = (ncc_err[filt]+ncc_err[filt-1])/2.;
	for(int i = 0; i < npoin; i++) {
		if (err_copy[i] > median) {
			ncc_pass[i] = 1;		
		}
		else {
			ncc_pass[i] = 0;
		}
	}	
}

void fb_filter(const CvPoint2D32f* prev_features, const CvPoint2D32f* backward_features, 
				const CvPoint2D32f* curr_feat, int *fb_pass, const int npoin) {
	//this function implements forward-backward error filtering
	vector<double> euclidean_dist (npoin,0.0);
	
	int filt = npoin/2;
	for(int i = 0; i < npoin; i++) {
		euclidean_dist[i] = euclid_dist(&prev_features[i], &backward_features[i]);
	}
	
	vector<double> err_copy (euclidean_dist);
	//use the STL sort algorithm to filter results
	sort(euclidean_dist.begin(), euclidean_dist.end());
	double median = (euclidean_dist[filt]+euclidean_dist[filt-1])/2.;
	for(int i = 0; i < npoin; i++) {
		if (err_copy[i] < median) {
			fb_pass[i] = 1;		
		}
		else {
			fb_pass[i] = 0;
		}
	}
}
void bbox_move(const CvPoint2D32f* prev_feat, const CvPoint2D32f* curr_feat, const int npoin,
				double &xmean, double &ymean) {
	/*Calculate bounding box motion. */
	vector<double> xvec (npoin,0.0);
	vector<double> yvec (npoin,0.0);
	for (int i = 0; i < npoin; i++) {
		xvec[i] = curr_feat[i].x - prev_feat[i].x;
		yvec[i] = curr_feat[i].y - prev_feat[i].y;
	}	
	
	sort(xvec.begin(), xvec.end());
	sort(yvec.begin(), yvec.end());
	
	xmean = xvec[npoin/2];
	ymean = yvec[npoin/2];
}


int main(int argc, char *argv[])
{
	
	CvCapture *input_video = cvCaptureFromFile( "camera6.avi" );
	if (input_video == NULL)
	{
		fprintf(stderr, "Error: Can't open video.\n");
		return -1;
	}

	static IplImage *frame = NULL;
	frame = cvQueryFrame( input_video );

	/* Read the video's frame size out of the AVI. */
	CvSize frame_size;
	frame_size.height =(int) cvGetCaptureProperty( input_video, CV_CAP_PROP_FRAME_HEIGHT );
	frame_size.width =(int) cvGetCaptureProperty( input_video, CV_CAP_PROP_FRAME_WIDTH );

	cvNamedWindow("Optical Flow", CV_WINDOW_AUTOSIZE);

	/* Preparation: Allocate the necessary storage. */
	static IplImage *frame2_1C = NULL, *pyramid1 = NULL, *pyramid2 = NULL, *frame1_1C = NULL;
	allocateOnDemand( &frame1_1C, frame_size, IPL_DEPTH_8U, 1 );
	allocateOnDemand( &frame2_1C, frame_size, IPL_DEPTH_8U, 1 );

	cvConvertImage(frame, frame1_1C, 0);
	double start, end,t1, t2;	
	int npoints = 100;
	int pcount;
	int winsize = 30;
	IplImage *rec0 = cvCreateImage( cvSize(winsize, winsize), 8, 1 );
	IplImage *rec1 = cvCreateImage( cvSize(winsize, winsize), 8, 1 );
	IplImage *res  = cvCreateImage( cvSize( 1, 1 ), IPL_DEPTH_32F, 1 );
	IplImage* displayed_img = NULL;
	allocateOnDemand( &displayed_img, frame_size, IPL_DEPTH_8U, 3 );
	cvConvertImage(frame, displayed_img);
	// Set up the callback
	cvSetMouseCallback( "Optical Flow", my_mouse_callback, (void*) displayed_img);
	/* This array will contain the locations of the points from frame 1 in frame 2. */
	vector<CvPoint2D32f> frame1_features(npoints);
	vector<CvPoint2D32f> frame2_features(npoints);
	vector<CvPoint2D32f> FB_features(npoints);
	/* The i-th element of this array will be non-zero if and only if the i-th feature of
	 * frame 1 was found in frame 2.
	 */
	char* optical_flow_found_feature = new char[npoints];
	char* optical_flow_found_feature2  = new char[npoints];
	float* optical_flow_feature_error = new float[npoints];
	vector<int> fb_pass(npoints);
	vector<int> ncc_pass(npoints);

	// Mouse select loop
	while( true ){
		if (!drawn_box) cvCopy( frame, displayed_img );
		if( drawing_box ) 
			cvRectangle( displayed_img, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),
						cvScalar(0xff,0x00,0x00) );
		cvShowImage( "Optical Flow", displayed_img );

		char key_pressed;
		key_pressed = cvWaitKey(33);
		
		if( key_pressed == 32 && drawn_box) {
			break;
		}
		if ( key_pressed == 27 && !drawn_box) {
			fprintf(stderr, "Terminated by user.\n");
			exit(-1);			
		}
	}

	double bbox_x = box.x;
	double bbox_y = box.y;
	double bbox_width = box.width;
	double bbox_height = box.height;


	//Main loop	
	while(true)
	{
		start= (double)cvGetTickCount();
		//create the grid of points		
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				int l = i*10+j;
				frame1_features[l].x = bbox_x + (bbox_width/10)*(j) + (bbox_width/20);
				frame1_features[l].y = bbox_y + (bbox_height/10)*(i) + (bbox_height/20);
			}
		}

		cvConvertImage(frame, frame1_1C, 0);

		// Get the second frame of video.
		frame = cvQueryFrame( input_video );
		if (frame == NULL)
		{
			return 0;
		}
		
		cvConvertImage(frame, frame2_1C, 0);
		cvCopy( frame, displayed_img );
		/* Pyramidal Lucas Kanade Optical Flow! */

		/* This is the window size to use to avoid the aperture problem (see slide "Optical Flow: Overview"). */
		CvSize optical_flow_window = cvSize(3,3);
	
		CvTermCriteria optical_flow_termination_criteria
			= cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

		allocateOnDemand( &pyramid1, frame_size, IPL_DEPTH_8U, 1 );
		allocateOnDemand( &pyramid2, frame_size, IPL_DEPTH_8U, 1 );

		/* Actually run Pyramidal Lucas Kanade Optical Flow
		 */
		cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2, &frame1_features[0], &frame2_features[0], 
								npoints, optical_flow_window, 5, optical_flow_found_feature, 
								optical_flow_feature_error, optical_flow_termination_criteria, 0 );
		//Reverse the LK tracker and determine tracking failures
		cvCalcOpticalFlowPyrLK(frame2_1C, frame1_1C, pyramid1, pyramid2, &frame2_features[0], &FB_features[0], 
						npoints, optical_flow_window, 5, optical_flow_found_feature2, 
						optical_flow_feature_error, optical_flow_termination_criteria, 0 );
		
		double xmean = 0;
		double ymean = 0;
		//filter cascade
		fb_filter(&frame1_features[0], &FB_features[0], &frame2_features[0], &fb_pass[0], npoints);

		ncc_filter(frame1_1C,frame2_1C,&frame1_features[0],&frame2_features[0],
				   npoints,CV_TM_CCOEFF_NORMED, rec0, rec1, res, &ncc_pass[0]);
		
		pcount = 0;		   
		for (int i = 0; i < npoints; i++) {
			if (ncc_pass[i] && fb_pass[i]) {
				pcount++;
			}
		}
		fprintf(stderr, "pcount: %d\n", pcount);
		vector<CvPoint2D32f> curr_features2(pcount), prev_features2(pcount);
		int j = 0;
		for (int i = 0; i < npoints; i++) {
			if (ncc_pass[i] && fb_pass[i]) {
				curr_features2[j] = frame2_features[i];
				prev_features2[j] = frame1_features[i];
				j++;
			}
		}		

		int n2 = pcount * pcount;
		vector<double> pdist_prev(n2), pdist_curr(n2), pdiv(n2);
		pairwise_dist(&prev_features2[0], &pdist_prev[0], pcount);
		pairwise_dist(&curr_features2[0], &pdist_curr[0], pcount);		
		
		for (int i = 0; i < n2; i++) {
			if (pdist_prev[i] > 0.0) {
				pdiv[i] = pdist_curr[i]/pdist_prev[i];
			}
		}
		sort(pdiv.begin(),pdiv.end());
		//fprintf(stderr, "box_scale: %3.4f", pdiv[n2/2]);	
		
		double box_scale;
		box_scale = pdiv[n2/2];
				  
		bbox_move(&prev_features2[0],&curr_features2[0],pcount,xmean,ymean);
		bbox_x = bbox_x + (xmean) - bbox_width*(box_scale - 1.)/2.;
		bbox_y = bbox_y + (ymean) - bbox_height*(box_scale - 1.)/2.;
		bbox_width = bbox_width * (box_scale);
		bbox_height = bbox_height * (box_scale);

		cvRectangle(displayed_img, cvPoint(bbox_x, bbox_y), 
					cvPoint(bbox_x+bbox_width,bbox_y+bbox_height),
					cvScalar(0xff,0x00,0x00) );
		for (int i = 0; i < pcount; i++) {
			cvCircle(displayed_img, cvPoint(curr_features2[i].x, curr_features2[i].y), 1, cvScalar(255,255,255));
		}
		char online_dist[] = "0.0";
		CvFont bfont;
		double hscale = 0.5;
		double vscale = 0.5;
		cvInitFont(&bfont, CV_FONT_HERSHEY_SIMPLEX, hscale, vscale,0,1);
		cvPutText(displayed_img, online_dist, cvPoint(bbox_x+bbox_width + 20,bbox_y), &bfont, cvScalar(0,0,255));
		
		cvShowImage("Optical Flow", displayed_img);
		end= (double)cvGetTickCount();
		char key_pressed = cvWaitKey(330);
		if( key_pressed == 27 ) {
			break;
		}
		t1= (end-start)/((double)cvGetTickFrequency()*1000.);
		fprintf(stderr, "time elapsed:  = %g ms\n", t1 );
	}
	cvReleaseImage(&displayed_img);
	cvReleaseImage(&frame1_1C);
	cvReleaseImage(&frame2_1C);
	cvReleaseImage(&pyramid1);
	cvReleaseImage(&pyramid2);
	cvReleaseImage(&rec0);
	cvReleaseImage(&rec1);
	cvReleaseImage(&res);
	cvReleaseCapture( &input_video );
	cvDestroyAllWindows();
}