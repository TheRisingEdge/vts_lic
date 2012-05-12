#include "PSubtractor.h"

void PSubtractor::run()
{
	long long frameCount = 0;
	while (true)
	{
		Mat frame = receive(_vidInput);
		if(frameCount < _trainingFrames)
		{
			_subtractor->learn(frame);
			frameCount++;
		}else
		{			
			Mat foregorund = _subtractor->segment(frame);	
			SubFrame result = {frame, foregorund.clone()};

			//imshow("frame", frame);
			//imshow("fg", foregorund);
			//waitKey();

			send(_target, result);			
		}				
	}
}

