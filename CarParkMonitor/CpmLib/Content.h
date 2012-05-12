#pragma once
#include <iostream>
#include "opencv2\core\core.hpp"

using namespace cv;

class Content
{
public:
	Content(){}
	~Content(){}

	static char* pathTo(char* pathReg, int index);
	static char* pathTo(char* pathReg, char* fileName);

	static char* image(char* name);
	static char* file(char* name);
	static char* ymlFile(char* name);
	static char* videoFile(char* name);
	static FileStorage read(char* file);
	static FileStorage write(char* file);

};

