#include "opencv2/core/core.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>

using namespace std;
using namespace cv;

int main()
{
	const char* path = "./../CarParkMonitor/Content/Assets/hogsvm.yml";
	FileStorage f = FileStorage(path, FileStorage::READ);
	Mat plane;
	f["hplane"] >> plane;
	
	cout << plane;
	getch();
	return 0;
}