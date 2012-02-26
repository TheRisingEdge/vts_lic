

#include "opencv2\video\tracking.hpp"
#include <time.h>
#include "opencv2\legacy\blobtrack.hpp"
#include "opencv2\opencv.hpp"

using namespace cv;

/* Get frequency for each module time working estimation: */
static double FREQ = 1000*cvGetTickFrequency();

#if 1
#define COUNTNUM 100
#define TIME_BEGIN() \
{\
    static double   _TimeSum = 0;\
    static int      _Count = 0;\
    static int      _CountBlob = 0;\
    int64           _TickCount = cvGetTickCount();\

#define TIME_END(_name_,_BlobNum_)    \
    _Count++;\
    _CountBlob+=_BlobNum_;\
    _TimeSum += (cvGetTickCount()-_TickCount)/FREQ;\
    if(m_TimesFile)if(_Count%COUNTNUM==0)\
    { \
        FILE* out = fopen(m_TimesFile,"at");\
        if(out)\
        {\
            fprintf(out,"ForFrame Frame: %d %s %f on %f blobs\n",_Count,_name_, _TimeSum/COUNTNUM,((float)_CountBlob)/COUNTNUM);\
            if(_CountBlob>0)fprintf(out,"ForBlob  Frame: %d %s - %f\n",_Count,_name_, _TimeSum/_CountBlob);\
            fclose(out);\
        }\
        _TimeSum = 0;\
        _CountBlob = 0;\
    }\
}
#else
#define TIME_BEGIN()
#define TIME_END(_name_)
#endif

/* Special extended blob structure for auto blob tracking: */
typedef struct CvBlobTrackAuto
{
	CvBlob  blob;
	int     BadFrames;
} CvBlobTrackAuto;


class AutoTracker: public CvBlobTrackerAuto
{
public:
    AutoTracker(CvBlobTrackerAutoParam1* param);
    ~AutoTracker();
    CvBlob* GetBlob(int index){return m_BlobList.GetBlob(index);};
    CvBlob* GetBlobByID(int ID){return m_BlobList.GetBlobByID(ID);};
    int     GetBlobNum(){return m_BlobList.GetBlobNum();};
    virtual IplImage* GetFGMask(){return m_pFGMask;};
    float   GetState(int BlobID){return m_pBTA?m_pBTA->GetState(BlobID):0;};
    const char*   GetStateDesc(int BlobID){return m_pBTA?m_pBTA->GetStateDesc(BlobID):NULL;};
    /* Return 0 if trajectory is normal;
       return >0 if trajectory abnormal. */
    void Process(IplImage* pImg, IplImage* pMask = NULL);
    void Release(){delete this;};

private:
    IplImage*               m_pFGMask;
    int                     m_FGTrainFrames;
    CvFGDetector*           m_pFG; /* Pointer to foreground mask detector module. */
    CvBlobTracker*          m_pBT; /* Pointer to Blob tracker module. */
    int                     m_BTDel;
    int                     m_BTReal;
    CvBlobDetector*         m_pBD; /* Pointer to Blob detector module. */
    int                     m_BDDel;
    CvBlobTrackGen*         m_pBTGen;
    CvBlobTrackPostProc*    m_pBTPostProc;
    int                     m_UsePPData;
    CvBlobTrackAnalysis*    m_pBTA; /* Blob trajectory analyser. */
    CvBlobSeq               m_BlobList;
    int                     m_FrameCount;
    int                     m_NextBlobID;
    const char*                   m_TimesFile;

public:
    virtual void SaveState(CvFileStorage* fs)
    {
        cvWriteInt(fs,"FrameCount",m_FrameCount);
        cvWriteInt(fs,"NextBlobID",m_NextBlobID);
        m_BlobList.Write(fs,"BlobList");
    };

    virtual void LoadState(CvFileStorage* fs, CvFileNode* node)
    {
        CvFileNode* BlobListNode = cvGetFileNodeByName(fs,node,"BlobList");
        m_FrameCount = cvReadIntByName(fs,node, "FrameCount", m_FrameCount);
        m_NextBlobID = cvReadIntByName(fs,node, "NextBlobID", m_NextBlobID);
        if(BlobListNode)
        {
            m_BlobList.Load(fs,BlobListNode);
        }
    };
};