#include "Content.h"


Content::Content(void)
{
}

Content::~Content(void)
{
}

char* Content::pathTo( char* pathReg, int id )
{
	char* fileName = new char[50];
	sprintf(fileName,pathReg, id);
	return fileName;
}

char* Content::pathTo( char* pathReg, char* file )
{
	char* fileName = new char[50];
	sprintf(fileName,pathReg, file);
	return fileName;
}

char* Content::file( char* name )
{
	return pathTo("./Content/Assets/%s", name);
}

char* Content::ymlFile( char* name )
{
	return pathTo("./Content/Assets/%s.yml", name);
}
