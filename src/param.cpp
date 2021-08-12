#include "param.h"

Param::Param(std::string key, std::string value) : key(key), value(value)
{}

Param::~Param()
{}

bool
Param::is_letter(char character)
{
	return ( (tolower(character) >= 'a') || (tolower(character) <= 'z') );
}
bool
Param::is_number(char character)
{
	return ( (character >= '0') || (character <= '9') );
}

int
Param::getHeaderSize(char * receiveBuffer)
{
	int size = strlen(receiveBuffer);
	int count = 0;
	for(int i = 0; i < size; ++i)
	{
		if(i+1 != size)
		{
			if((receiveBuffer[i] == '\n') && (receiveBuffer[i+1] == '\n'))
			{
				break;
			}
		}
		count += 1;
	}
	return count;	
}

int
Param::getParamSize(char * receiveBuffer)
{	
	// start at end of receiveBuffer and keep counting backwards until encountering a new line
	// newline specifies the start of the request body containing the key and value parameters
	// return the counted number of characters
	int count = 0;
	for(int i = strlen(receiveBuffer); receiveBuffer[i] != '\n'; i--)
	{
		count += 1;
	}
	return count;
}

void
Param::print()
{
	printf("key:value = {\"%s\":\"%s\"}\n", key.c_str(), value.c_str());
}

int
Param::getKeysAndValues(char * receiveBuffer, std::vector<Param *> & params)
{	
	try
	{
		// count the number of characters within the parameter section of the receiveBuffer
		// if there is only one character or less in it, there are no parameters present
		// do nothing; return 0
		unsigned int count = getParamSize(receiveBuffer);
		if(count <= 1)
		{
			printf("No params.\n"); fflush(stdout);
			return -1;
		}

		// delete and clear the keys and values from the vector
		for(unsigned int i = 0; i < params.size(); ++i)
		{
			delete params[i];
		}
		params.clear();

		std::string k;
		std::string v;
		bool isKey = true;

		// start reading from the beginning of the parameter list by subtracting the total 
		// length of the receiveBuffer with the number of characters within the parameter list
		// keep reading characters until the end of the receiveBuffer is reached
		for(unsigned int i = (strlen(receiveBuffer) - count); i < strlen(receiveBuffer); i++)
		{
			if( (receiveBuffer[i] == '{') || (receiveBuffer[i] == ' ') || (receiveBuffer[i] == '\t') || (receiveBuffer[i] == '\n') || (receiveBuffer[i] == '\"') || (receiveBuffer[i] == '\''))
			{
				continue;
			}
			// ending character; no more keys or values; exit loop
			else if(receiveBuffer[i] == '}')
			{
				if(k.size() && v.size())
				{
					params.push_back(new Param(k,v));
				}
				break;
			}
			else if(receiveBuffer[i] == ',')
			{
				isKey = true;
				if(k.size() && v.size())
				{
					params.push_back(new Param(k,v));
					k.clear();
					v.clear();
				}
			}
			// opening character for start of value
			else if(receiveBuffer[i] == ':')
			{
				isKey = false;
			}
			else if(isKey)
			{
				k += tolower(receiveBuffer[i]);
			}
			else
			{
				v += receiveBuffer[i];
			}
		}
	}
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
		return -1;
	}
	return 0;
}

int
Param::getHeader(char * receiveBuffer, std::vector<Param *> & params)
{	
	if(getHeaderSize(receiveBuffer) < 1)
		return -1;
	
	params.clear();
	
	std::string k;
	std::string v;
	
	bool isKey = true;
	
	// skip request
	int size = getHeaderSize(receiveBuffer);
	int skip = 0;
	bool isDone = false;
	for(int i = 0; i < size && !isDone; ++i)
	{
		if(receiveBuffer[i] == ':')
		{
			do
			{
				if(receiveBuffer[i] == '\n')
				{
					skip += 1;
					isDone = true;
					break;
				}
				skip -= 1;
				i -= 1;
			} while(i >= 0);
		}
		else
		{
			skip += 1;
		}
	}
	
	for(int i = skip; i < size; ++i)
	{
		if(isKey)
		{
			if(receiveBuffer[i] != ':')
			{
				k += (receiveBuffer[i]);
			}
			else
			{
				isKey = false;
			}
		}
		else
		{
			if(receiveBuffer[i] == ' ') continue;
			if(receiveBuffer[i] != '\n')
			{
				v += (receiveBuffer[i]);
			}
			else
			{
				params.push_back(new Param(k,v));
				k.clear();
				v.clear();
				isKey = true;
			}
		}
	}
	return 0;
}