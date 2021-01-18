#include "file_io.h"
/*
int 
FileIO::getFileSize(const char * file)
{
	try
	{
		FILE * FP = fopen(strlen(file) ? file : DEFAULT_FILE, "r");
		if(FP == nullptr)
		{
			throw "Could not open file.";
		}

		int i = 0;
		int c;
		
		while ((c = fgetc(FP)) != EOF)
		{
			i++;
		}
		if (feof(FP))
		{
			fclose(FP);
			return i;
		}
		throw "File error.";
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
	
	return -1;
}
*/
std::string
FileIO::getFileContent(const char * file)
{	
	try
	{
		
		printf("filePath: %s\n", file);
		
		FILE * FP = fopen(strlen(file) ? file : DEFAULT_FILE, "r");
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