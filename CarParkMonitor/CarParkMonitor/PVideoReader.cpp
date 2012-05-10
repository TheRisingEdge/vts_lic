#include "PVideoReader.h"
#include "Video.h"

void PVideoReader::run()
{
	int fps;
	double frameDelay;
	Video vid = Video(_videoPath);	
	if(!vid.openCapture(fps, frameDelay))
		throw std::exception("could not open video input");

	Mat frame;
	while(vid.read(frame))
	{
		receive(_syncBuffer);
		//imshow("frame", frame);
		//cv::waitKey(frameDelay);
		send(_outBuffer0, frame.clone());
		send(_outBuffer1, frame.clone());
	}
}
