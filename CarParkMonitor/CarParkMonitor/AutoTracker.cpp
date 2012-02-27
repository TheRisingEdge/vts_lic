#include "AutoTracker.h"

/* Constructor of auto blob tracker: */
AutoTracker::AutoTracker(CvBlobTrackerAutoParam1* param):trackedBlobs(sizeof(CvBlobTrackAuto))
{
    trackedBlobs.AddFormat("i");
    m_TimesFile = NULL;
    AddParam("TimesFile",&m_TimesFile);

    nextBlobId = 0;
    fgMask = NULL;
    frameCount = 0;

    trainingFramesCount = param?param->FGTrainFrames:0;
    fgDetector = param?param->pFG:0;

    m_BDDel = 0;
    blobDetector = param?param->pBD:NULL;

    m_BTDel = 0;
    blobTracker = param?param->pBT:NULL;;
    m_BTReal = blobTracker?blobTracker->IsModuleName("BlobTrackerReal"):0;

    blobTrackGenerator = param?param->pBTGen:NULL;

    blobTrackAnalyser = param?param->pBTA:NULL;

    blobTrackPostProc = param?param->pBTPP:NULL;
    m_UsePPData = param?param->UsePPData:0;

    /* Create default submodules: */
    if(blobDetector==NULL)
    {
        blobDetector = cvCreateBlobDetectorSimple();
        m_BDDel = 1;
    }

    if(blobTracker==NULL)
    {
        blobTracker = cvCreateBlobTrackerMS();
        m_BTDel = 1;
    }

    SetModuleName("Auto1");

} /* CvBlobTrackerAuto1::CvBlobTrackerAuto1 */

AutoTracker::~AutoTracker()
{
    if(m_BDDel)blobDetector->Release();
    if(m_BTDel)blobTracker->Release();
}

void AutoTracker::Process(IplImage* frame, IplImage* pMask)
{
    int         CurBlobNum = 0;
    int         i;    
	IplImage*	fgMask;
    frameCount++;


#pragma region fgDetector
/* Update BG model: */

	if(fgDetector)
	{   /* If FG detector is needed: */
		fgDetector->Process(frame);
		fgMask = fgDetector->GetMask();
	}   /* If FG detector is needed. */	

	if(fgDetector)
	{// debug foreground result
		IplImage *pFG = fgMask;
		if(pFG)
		{
			cvNamedWindow("FG",0);
			cvShowImage("FG", pFG);
		}
	}

#pragma endregion fgDetector


#pragma region blobDetection
	
  //  TIME_BEGIN()
    if(blobDetector && (frameCount > trainingFramesCount) )
    {   /* Detect new blob: */
        static CvBlobSeq    newPotentialBlobs;
        CvBlobTrackAuto     autoBlob;

        newPotentialBlobs.Clear();

		//add blobs to be tracked
        if(blobDetector->DetectNewBlob(frame, fgMask, &newPotentialBlobs, &trackedBlobs))
        {               
            //new blobs detected

		}

		//	int size = newPotentialBlobs.GetBlobNum();
  //          for(int i = 0; i < size; ++i)
  //          {
  //              CvBlob* potentialBlob = newPotentialBlobs.GetBlob(i);
  //              potentialBlob->ID = nextBlobId;

  //              if(potentialBlob->w >= CV_BLOB_MINW && potentialBlob->h >= CV_BLOB_MINH)
  //              {
  //                  CvBlob* trackedBlob = blobTracker->AddBlob(potentialBlob, frame, mask );
  //                  if(trackedBlob)
  //                  {
  //                      autoBlob.blob = *trackedBlob;
  //                      autoBlob.BadFrames = 0;
  //                      trackedBlobs.AddBlob((CvBlob*)&autoBlob);
  //                      nextBlobId++;
  //                  }
  //              }
  //          }
  //          if(mask != pFG) cvReleaseImage(&mask);
  //      }
	} 

  //  TIME_END("BlobDetector",-1)
#pragma endregion blobDetection


} /* CvBlobTrackerAuto1::Process */

#pragma region helpers

void AutoTracker::SaveState( CvFileStorage* fs )
{
	cvWriteInt(fs,"FrameCount",frameCount);
	cvWriteInt(fs,"NextBlobID",nextBlobId);
	trackedBlobs.Write(fs,"BlobList");
}

void AutoTracker::LoadState( CvFileStorage* fs, CvFileNode* node )
{
	CvFileNode* BlobListNode = cvGetFileNodeByName(fs,node,"BlobList");
	frameCount = cvReadIntByName(fs,node, "FrameCount", frameCount);
	nextBlobId = cvReadIntByName(fs,node, "NextBlobID", nextBlobId);
	if(BlobListNode)
	{
		trackedBlobs.Load(fs,BlobListNode);
	}
}

IplImage* AutoTracker::GetFGMask()
{
	return fgMask;
}

#pragma  endregion helpers

