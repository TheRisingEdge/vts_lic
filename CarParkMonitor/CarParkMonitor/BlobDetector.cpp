#include "BlobDetector.h"
#include "opencv2/opencv.hpp"

using namespace cv;


BlobDetector::BlobDetector()
{
	this->minArea = MIN_HEIGHT*MIN_WIDTH;	
}

BlobDetector::~BlobDetector()
{
}

int BlobDetector::DetectNewBlob( IplImage* iplFrame, IplImage* iplMask, CvBlobSeq* potentialBlobList, CvBlobSeq* trackedBlobs )
{

	Mat frame(iplFrame);
	Mat fgMask(iplMask);
	Mat maskCopy = fgMask.clone();

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( maskCopy, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	
	vector<Rect> boundingRectangles;
	int size = contours.size();
	for( int i = 0; i < size; i++ )
	{     
		Rect rect = boundingRect( Mat(contours[i]) );		
		if(rect.area() > this->minArea)
		{
			boundingRectangles.push_back(rect);
		}			
	}
	
	RNG rng(12345);	
	size = boundingRectangles.size();
	for( int i = 0; i< size; i++ )
	{
		Rect rect = boundingRectangles[i];
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		rectangle( frame, rect.tl(), rect.br(), color);
	}

	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	imshow( "Contours", frame );


	//int         i;
	//int         TestObjNum;
	//IplImage*   pMask = NULL;
	//IplImage*   pMaskCopy = NULL;
	//CvSeq*      cnts = NULL;

	//if(m_pTestSeq==NULL) return 0;

	//TestObjNum = cvTestSeqGetObjectNum(m_pTestSeq);
	//pMask = cvTestSeqGetFGMask(m_pTestSeq);

	//if(pMask == NULL) return 0;
	//pMaskCopy = cvCloneImage(pMask);
	//assert(pMaskCopy);

	//cvClearMemStorage(m_pMem);
	//cvFindContours( pMaskCopy, m_pMem, &cnts, sizeof(CvContour), CV_RETR_EXTERNAL);
	//cvReleaseImage(&pMaskCopy);

	//for(i=0; i<TestObjNum; ++i)
	//{   /* Check each object: */
	//	CvPoint2D32f    RealPos;
	//	CvPoint2D32f    RealSize;
	//	int             RealPosFlag = cvTestSeqGetObjectPos(m_pTestSeq,i,&RealPos);
	//	int             RealSizeFlag = cvTestSeqGetObjectSize(m_pTestSeq,i,&RealSize);

	//	if(!RealPosFlag) continue;
	//	if(m_DetectedBlobs.GetBlobByID(i)) continue;

	//	if(RealSizeFlag)
	//	{   /* Real size is known: */
	//		float W2 = RealSize.x * 0.5f;
	//		float H2 = RealSize.y * 0.5f;
	//		if( RealPos.x > W2 && RealPos.x < (pMask->width-W2) &&
	//			RealPos.y > H2 && RealPos.y < (pMask->height-H2) )
	//		{   /* Yes!!  We found new blob, let's add it to list: */
	//			CvBlob  NewBlob;
	//			NewBlob.x = RealPos.x;
	//			NewBlob.y = RealPos.y;
	//			NewBlob.w = RealSize.x;
	//			NewBlob.h = RealSize.y;
	//			NewBlob.ID = i;
	//			m_DetectedBlobs.AddBlob(&NewBlob);
	//			potentialBlobList->AddBlob(&NewBlob);
	//		}
	//	}   /* Real size is known. */
	//	else
	//	{
	//		CvSeq*  cnt;
	//		if(m_DetectedBlobs.GetBlobByID(i)) continue;

	//		for(cnt=cnts; cnt; cnt=cnt->h_next)
	//		{
	//			//CvBlob* pNewBlob = NULL;
	//			CvBlob  NewBlob;
	//			CvRect  r = cvBoundingRect( cnt );
	//			float   x = RealPos.x - r.x;
	//			float   y = RealPos.y - r.y;

	//			if(x<0 || x > r.width || y < 0 || y > r.height ) continue;

	//			if( r.x <= 1 ||
	//				r.y <= 1 ||
	//				r.x + r.width >= pMask->width - 2 ||
	//				r.y + r.height >= pMask->height - 2 ) continue;

	//			/* Yes!!  We found new blob, let's add it to list: */
	//			NewBlob.x = RealPos.x;
	//			NewBlob.y = RealPos.y;
	//			NewBlob.w = (float)r.width;
	//			NewBlob.h = (float)r.height;
	//			NewBlob.ID = i;
	//			m_DetectedBlobs.AddBlob(&NewBlob);
	//			potentialBlobList->AddBlob(&NewBlob);
	//		}
	//	}   /* Check new blob entrance. */
	//}   /* Check next object. */

	//return potentialBlobList->GetBlobNum();
	return 0;
}

void BlobDetector::Release()
{
	delete this;
}
