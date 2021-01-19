#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include <string.h>
#include <string>

class FileIO
{
private:
	const char * viewDir;
	char * defaultFile;
public:
	FileIO(const char * viewDir, const char * defaultFile);
	~FileIO();
	
	std::string getFileContent(const char * file);
};


#endif // FILE_IO_H