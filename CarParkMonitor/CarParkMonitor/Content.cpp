#include "Content.h"


Content::Content(void)
{
}

Content::~Content(void)
{
}

char* Content::pathTo( char* pathReg, int index )
{
	char* fileName = new char[50];
	sprintf(fileName,pathReg, index);
	return fileName;
}

char* Content::pathTo( char* pathReg, char* fileName )
{
	char* filePath = new char[50];
	sprintf(filePath,pathReg, fileName);
	return filePath;
}

char* Content::file( char* name )
{
	return pathTo("./Content/Assets/%s", name);
}

char* Content::ymlFile( char* name )
{
	return pathTo("./Content/Assets/%s.yml", name);
}

char* Content::videoFile( char* name )
{
	return pathTo("./Content/Videos/%s", name);
}
