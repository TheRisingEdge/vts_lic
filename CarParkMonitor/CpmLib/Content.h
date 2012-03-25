#pragma once
#include <iostream>

class Content
{
public:
	Content(void);
	~Content(void);

	static char* pathTo(char* pathReg, int index);
	static char* pathTo(char* pathReg, char* fileName);

	static char* file(char* name);
	static char* ymlFile(char* name);
	static char* videoFile(char* name);
};

