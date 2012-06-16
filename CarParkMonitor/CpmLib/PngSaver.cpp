#include "PngSaver.h"

char* PngSaver::basePath;
int PngSaver::count = 0;

void PngSaver::save( char* name, Mat image )
{
	char* fullPath = new char[100];
	sprintf(fullPath, "%s/%s-%d.png", basePath, name, count);
	imwrite(fullPath, image);
	delete[] fullPath;
}

void PngSaver::setBasePath( char* basePath )
{
	PngSaver::basePath = basePath;
}


