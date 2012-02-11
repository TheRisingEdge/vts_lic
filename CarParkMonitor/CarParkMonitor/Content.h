#pragma once
#include <iostream>

class Content
{
public:
	Content(void);
	~Content(void);

	static char* pathTo(char* pathReg, int id);

	static char* pathTo(char* pathReg, char* file);

	static char* file(char* name);

	static char* ymlFile(char* name);


};

