#include "file_io.h"

FileIO::FileIO(const char * viewDir, const char * defaultFile) : viewDir(viewDir)
{
	this->defaultFile = (char *) calloc(strlen(viewDir) + strlen(defaultFile) + 1, sizeof(char));
	strcpy(this->defaultFile, viewDir);
	strcat(this->defaultFile, defaultFile);
}
FileIO::~FileIO()
{
	free(this->defaultFile);
}

std::string
FileIO::getFileContent(const char * file)
{	
	try
	{
		
		printf("filePath: %s\n", file);
		
		FILE * FP = fopen(strcmp(file, viewDir) ? file : defaultFile, "r");
		if(FP == nullptr)
		{
			throw "Could not open file.";
		}
		
		std::string output = "";
		while(!feof(FP))
		{
			output += fgetc(FP);
		}
		if (feof(FP))
		{
			// remove EOF
			output.pop_back();
			fclose(FP);
		}
		else
		{
			throw "File error.";
		}
		return output;
	}
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
	return "COULD_NOT_LOAD_PAGE";
}