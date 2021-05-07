#include "param.h"

Param::Param(unsigned int paramSize) : paramSize(paramSize)
{}

Param::~Param()
{}

bool
Param::is_letter(char character)
{
	return ( ( (tolower(character) < ASCII::LOWER_LETTER_A) || (tolower(character) > ASCII::LOWER_LETTER_Z) ) ? false : true );
}
bool
Param::is_number(char character)
{
	return ( ( (character < ASCII::STR_ZERO) || (character > ASCII::STR_NINE) ) ? false : true );
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
	for(unsigned int i = 0; i < keys.size(); ++i)
	{
		printf("key_value[%d] = {\"%s\":\"%s\"}\n", i, keys[i], values[i]);
	}
	printf("\n");
}

void
Param::printHeader()
{
	for(unsigned int i = 0; i < keyz.size(); ++i)
	{
		printf("k_v[%d] = %s : %s\n", i, keyz[i].c_str(), valuez[i].c_str());
	}
	printf("\n");
}

int
Param::getKeyAndValue(char * receiveBuffer)
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
		
		// flags specifying whether characters are part of a key or a value
		// set initial values to false
		bool isKey = false;
		bool isValue = false;

		// first delete all allocated pointers, since vector::clear() does not do that for you
		for(unsigned int i = 0; i < keys.size(); ++i)
		{
			free(keys[i]);
		}
		for(unsigned int i = 0; i < values.size(); ++i)
		{
			free(values[i]);
		}
		// clear the keys and values vectors
		keys.clear();
		values.clear();

		// allocate memory for a key and value pair with size paramSize
		// throw exception if no memory could be allocated (returns nullptr)
		char * key = (char *) calloc(paramSize, sizeof(char));
		char * value = (char *) calloc(paramSize, sizeof(char));
		if(key == nullptr)
		{
			throw "Could not allocate memory for key.";
		}
		if(value == nullptr)
		{
			throw "Could not allocate memory for value.";
		}

		// index within the key and value vectors
		unsigned int key_vec_i = 0;
		unsigned int value_vec_i = 0;

		// index within the key and value char *'s
		unsigned int key_i = 0;
		unsigned int value_i = 0;

		// start reading from the beginning of the parameter list by subtracting the total 
		// length of the receiveBuffer with the number of characters within the parameter list
		// keep reading characters until the end of the receiveBuffer is reached
		for(unsigned int i = (strlen(receiveBuffer) - count); i < strlen(receiveBuffer); i++)
		{
			if(isKey)
			{
				// make sure program does not reach part where character is copied to key if
				// index of key is equal to or surpasses the paramSize; program will attempt to
				// write to memory out of bounds and the OS will terminate the process; program 
				// will explode, maybe
				if(key_i >= paramSize)
				{
					// free memory allocated for key and value
					free(key);
					free(value);
					throw "Key size is too large.";
				}
				
				// closing quotation mark of key; no more key characters;
				// set isKey to false
				if(receiveBuffer[i] == '\"')
				{
					isKey = false;
					
					// push empty char * to keys vector
					// copy contents of key into empty char *
					// reset key to 0
					keys.push_back((char *) calloc(paramSize, sizeof(char)));
					strcpy(keys[key_vec_i], key);
					memset(key, 0, paramSize);

					// increment keys vector index with one
					// reset key index to 0
					key_vec_i += 1;
					key_i = 0;
					
					// skip rest of loop; return to top
					continue;
				}
				
				// convert character from buffer to lower case and add it to key
				// increment key index with 1
				key[key_i] = tolower(receiveBuffer[i]);
				key_i += 1;
			}
			else if(isValue)
			{
				// make sure program does not reach part where character is copied to value if
				// index of value is equal to or surpasses the paramSize; program will attempt to 
				// write to memory out of bounds and the OS will terminate the process; program 
				// will explode, maybe
				if(value_i >= paramSize)
				{
					// free memory allocated for key and value
					free(key);
					free(value);
					throw "Value size is too large.";
				}
				
				// closing quotation mark of value; no more value characters;
				// set isValue to false
				if(receiveBuffer[i] == '\"')
				{
					isValue = false;
					
					// push empty char * to values vector
					// copy contents of value into empty char *
					// reset value to 0
					values.push_back((char *) calloc(paramSize, sizeof(char)));
					strcpy(values[value_vec_i], value);
					memset(value, 0, paramSize);
					
					// increment values vector index with one
					// reset value index to 0
					value_vec_i += 1;
					value_i = 0;

					// skip rest of loop; return to top
					continue;
				}
				
				// convert character from buffer to lower case and add it to value
				// increment value index with 1
				value[value_i] = receiveBuffer[i];
				//value[value_i] = tolower(receiveBuffer[i]);
				value_i += 1;
			}

			// opening character for start of key
			// set isKey to true and skip the following quotation mark by incrementing i with 1
			if( (receiveBuffer[i] == '{') || (receiveBuffer[i] == ',') )
			{
				isKey = true;
				i += 1;
			}
			// opening character for start of value
			// set isValue to true and skip the following quotation mark by incrementing i with 1
			else if(receiveBuffer[i] == ':')
			{
				isValue = true;
				i += 1;
			}
			// ending character; no more keys or values; exit loop
			else if(receiveBuffer[i] == '}')
			{
				break;
			}
		}
		// free memory allocated for key and value
		free(key);
		free(value);
	}
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
		return -1;
	}	
	return 0;
}

int
Param::getHeader(char * receiveBuffer)
{
	//printf("header size: %d\n", getHeaderSize(receiveBuffer));
	
	if(getHeaderSize(receiveBuffer) < 1)
		return -1;
	
	keyz.clear();
	valuez.clear();
	
	std::string key;
	std::string value;
	
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
				key += (receiveBuffer[i]);
			}
			else
			{
				keyz.push_back(key);
				key.clear();
				isKey = false;
			}
		}
		else
		{
			if(receiveBuffer[i] == ' ') continue;
			if(receiveBuffer[i] != '\n')
			{
				value += (receiveBuffer[i]);
			}
			else
			{
				valuez.push_back(value);
				value.clear();
				isKey = true;
			}
		}
	}
	
	for(unsigned int i = 0; i < keyz.size(); ++i)
	{
		if(strcmp("authorization",keyz[i].c_str())==0)
		{
			printf("value: %s\n", valuez[i].c_str());
		}
	}
	
	return 0;
}