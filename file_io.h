#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include <string.h>

class FileIO
{
private:
	const char * DEFAULT_FILE = "index.html";
public:
	FileIO()
	{}
	
	int getFileSize(const char * file);
	void getFileContent(const char * file, char * output);
};


#endif // FILE_IO_H