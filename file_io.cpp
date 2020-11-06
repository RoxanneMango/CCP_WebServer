#include "file_io.h"

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

void
FileIO::getFileContent(const char * file, char * output)
{	
	try
	{
		FILE * FP = fopen(strlen(file) ? file : DEFAULT_FILE, "r");
		if(FP == nullptr)
		{
			throw "Could not open file.";
		}
		
		int i = 0;
		char c;
		while ((c = fgetc(FP)) != EOF)
		{
				*(output + i) = c;
				i += 1;
		}
		if (feof(FP))
		{
			//*(output + i) = c;
			fclose(FP);
			return;
		}
		throw "File error.";
	}
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}