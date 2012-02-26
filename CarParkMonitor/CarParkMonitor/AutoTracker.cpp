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

    m_FGTrainFrames = param?param->FGTrainFrames:0;
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


void AutoTracker::Process(IplImage* pImg, IplImage* pMask)
{
    int         CurBlobNum = 0;
    int         i;
    IplImage*   pFG = pMask;

    /* Bump frame counter: */
    frameCount++;

    if(m_TimesFile)
    {
        static int64  TickCount = cvGetTickCount();
        static double TimeSum = 0;
        static int Count = 0;
        Count++;

        if(Count%100==0)
        {
#ifndef WINCE
            time_t ltime;
            time( &ltime );
			char* stime = ctime( &ltime );
#else
			/* WINCE does not have above POSIX functions (time,ctime) */
			const char* stime = " wince ";
#endif
            FILE* out = fopen(m_TimesFile,"at");
            double Time;
            TickCount = cvGetTickCount()-TickCount;
            Time = TickCount/FREQ;
            if(out){fprintf(out,"- %sFrame: %d ALL_TIME - %f\n",stime,Count,Time/1000);fclose(out);}

            TimeSum = 0;
            TickCount = cvGetTickCount();
        }
    }

#pragma region fgDetector
/* Update BG model: */
TIME_BEGIN()

	if(fgDetector)
	{   /* If FG detector is needed: */
		fgDetector->Process(pImg);
		pFG = fgDetector->GetMask();
	}   /* If FG detector is needed. */

	TIME_END("FGDetector",-1)

		fgMask = pFG; /* For external use. */

	//if(m_pFG && m_pFG->GetParam("DebugWnd") == 1)
	{// debug foreground result
		IplImage *pFG = fgDetector->GetMask();
		if(pFG)
		{
			cvNamedWindow("FG",0);
			cvShowImage("FG", pFG);
		}
	}

#pragma endregion fgDetector

#pragma region blobTracker
/* Track blobs: */
TIME_BEGIN()
	if(blobTracker)
	{
		int i;
		blobTracker->Process(pImg, pFG);

		for(i=trackedBlobs.GetBlobNum(); i>0; --i)
		{   /* Update data of tracked blob list: */
			CvBlob* pB = trackedBlobs.GetBlob(i-1);
			int     BlobID = CV_BLOB_ID(pB);
			int     i = blobTracker->GetBlobIndexByID(BlobID);
			blobTracker->ProcessBlob(i, pB, pImg, pFG);
			pB->ID = BlobID;
		}
		CurBlobNum = blobTracker->GetBlobNum();
	}
	TIME_END("BlobTracker",CurBlobNum)
#pragma endregion blobTracker

    /* This part should be removed: */
    if(m_BTReal && blobTracker)
    {   /* Update blob list (detect new blob for real blob tracker): */
        int i;

        for(i=blobTracker->GetBlobNum(); i>0; --i)
        {   /* Update data of tracked blob list: */
            CvBlob* pB = blobTracker->GetBlob(i-1);
            if(pB && trackedBlobs.GetBlobByID(CV_BLOB_ID(pB)) == NULL )
            {
                CvBlobTrackAuto  NewB;
                NewB.blob = pB[0];
                NewB.BadFrames = 0;
                trackedBlobs.AddBlob((CvBlob*)&NewB);
            }
        }   /* Next blob. */

        /* Delete blobs: */
        for(i=trackedBlobs.GetBlobNum(); i>0; --i)
        {   /* Update tracked-blob list: */
            CvBlob* pB = trackedBlobs.GetBlob(i-1);
            if(pB && blobTracker->GetBlobByID(CV_BLOB_ID(pB)) == NULL )
            {
                trackedBlobs.DelBlob(i-1);
            }
        }   /* Next blob. */
    }   /* Update bloblist. */

#pragma region blobTrackPostProcessing
	TIME_BEGIN()
	if(blobTrackPostProc)
	{   /* Post-processing module: */
		int i;
		for(i=trackedBlobs.GetBlobNum(); i>0; --i)
		{   /* Update tracked-blob list: */
			CvBlob* autoBlob = trackedBlobs.GetBlob(i-1);
			blobTrackPostProc->AddBlob(autoBlob);
		}
		blobTrackPostProc->Process();

		for(i=trackedBlobs.GetBlobNum(); i>0; --i)
		{   /* Update tracked-blob list: */
			CvBlob* pB = trackedBlobs.GetBlob(i-1);
			int     BlobID = CV_BLOB_ID(pB);
			CvBlob* pBN = blobTrackPostProc->GetBlobByID(BlobID);

			if(pBN && m_UsePPData && pBN->w >= CV_BLOB_MINW && pBN->h >= CV_BLOB_MINH)
			{   /* Set new data for tracker: */
				blobTracker->SetBlobByID(BlobID, pBN );
			}

			if(pBN)
			{   /* Update blob list with results from postprocessing: */
				pB[0] = pBN[0];
			}
		}
	}   /* Post-processing module. */

	TIME_END("PostProcessing",CurBlobNum)
#pragma endregion blobTrackPostProcessing

    /* Blob deleter (experimental and simple): */
    TIME_BEGIN()
    if(pFG)
    {   /* Blob deleter: */
        int i;
        if(!m_BTReal)for(i=trackedBlobs.GetBlobNum();i>0;--i)
        {   /* Check all blobs on list: */
            CvBlobTrackAuto* autoBlob = (CvBlobTrackAuto*)(trackedBlobs.GetBlob(i-1));
            int     Good = 0;
            int     w=pFG->width;
            int     h=pFG->height;
            CvRect  r = CV_BLOB_RECT(autoBlob);
            CvMat   mat;
            double  aver = 0;
            double  area = CV_BLOB_WX(autoBlob)*CV_BLOB_WY(autoBlob);
            if(r.x < 0){r.width += r.x;r.x = 0;}
            if(r.y < 0){r.height += r.y;r.y = 0;}
            if(r.x+r.width>=w){r.width = w-r.x-1;}
            if(r.y+r.height>=h){r.height = h-r.y-1;}

            if(r.width > 4 && r.height > 4 && r.x < w && r.y < h &&
                r.x >=0 && r.y >=0 &&
                r.x+r.width < w && r.y+r.height < h && area > 0)
            {
                aver = cvSum(cvGetSubRect(pFG,&mat,r)).val[0] / area;
                /* if mask in blob area exists then its blob OK*/
                if(aver > 0.1*255)Good = 1;
            }
            else
            {
                autoBlob->BadFrames+=2;
            }

            if(Good)
            {
                autoBlob->BadFrames = 0;
            }
            else
            {
                autoBlob->BadFrames++;
            }
        }   /* Next blob: */

        /* Check error count: */
        for(i=0; i<trackedBlobs.GetBlobNum(); ++i)
        {
            CvBlobTrackAuto* pB = (CvBlobTrackAuto*)trackedBlobs.GetBlob(i);

            if(pB->BadFrames>3)
            {   /* Delete such objects */
                /* from tracker...     */
                blobTracker->DelBlobByID(CV_BLOB_ID(pB));

                /* ... and from local list: */
                trackedBlobs.DelBlob(i);
                i--;
            }
        }   /* Check error count for next blob. */
    }   /*  Blob deleter. */

    TIME_END("BlobDeleter",trackedBlobs.GetBlobNum())

    /* Update blobs: */
    TIME_BEGIN()
    if(blobTracker)
        blobTracker->Update(pImg, pFG);
    TIME_END("BlobTrackerUpdate",CurBlobNum)

    /* Detect new blob: */
    TIME_BEGIN()
    if(!m_BTReal && blobDetector && pFG && (frameCount > m_FGTrainFrames) )
    {   /* Detect new blob: */
        static CvBlobSeq    NewBlobList;
        CvBlobTrackAuto     autoBlob;

        NewBlobList.Clear();

        if(blobDetector->DetectNewBlob(pImg, pFG, &NewBlobList, &trackedBlobs))
        {   /* Add new blob to tracker and blob list: */
            int i;
            IplImage* mask = pFG;

            /*if(0)if(NewBlobList.GetBlobNum()>0 && pFG )
            {// erode FG mask (only for FG_0 and MS1||MS2)
                pMask = cvCloneImage(pFG);
                cvErode(pFG,pMask,NULL,2);
            }*/

            for(i=0; i<NewBlobList.GetBlobNum(); ++i)
            {
                CvBlob* blob = NewBlobList.GetBlob(i);
                blob->ID = nextBlobId;

                if(blob && blob->w >= CV_BLOB_MINW && blob->h >= CV_BLOB_MINH)
                {
                    CvBlob* trackedBlob = blobTracker->AddBlob(blob, pImg, mask );
                    if(trackedBlob)
                    {
                        autoBlob.blob = *trackedBlob;
                        autoBlob.BadFrames = 0;
                        trackedBlobs.AddBlob((CvBlob*)&autoBlob);
                        nextBlobId++;
                    }
                }
            }   /* Add next blob from list of detected blob. */

            if(mask != pFG) cvReleaseImage(&mask);

        }   /* Create and add new blobs and trackers. */

    }   /*  Detect new blob. */

    TIME_END("BlobDetector",-1)

#pragma region track_generator
	TIME_BEGIN()
	if(blobTrackGenerator)
	{   /* Run track generator: */
		for(i=trackedBlobs.GetBlobNum(); i>0; --i)
		{   /* Update data of tracked blob list: */
			CvBlob* autoBlob = trackedBlobs.GetBlob(i-1);
			blobTrackGenerator->AddBlob(autoBlob);
		}
		blobTrackGenerator->Process(pImg, pFG);
	}   /* Run track generator: */
	TIME_END("TrajectoryGeneration",-1)
#pragma endregion track_generator
    
#pragma region trajectory_analysis
		TIME_BEGIN()
		if(blobTrackAnalyser)
		{   /* Trajectory analysis module: */
			int i;
			for(i=trackedBlobs.GetBlobNum(); i>0; i--)
				blobTrackAnalyser->AddBlob(trackedBlobs.GetBlob(i-1));

			blobTrackAnalyser->Process(pImg, pFG);

		}   /* Trajectory analysis module. */

		TIME_END("TrackAnalysis",trackedBlobs.GetBlobNum())
#pragma endregion trajectory_analysis


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

