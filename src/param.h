#ifndef PARAM_H
#define PARAM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include <vector>
#include <string>

class Param
{
private:
	unsigned int paramSize;
	
	static int getParamSize(char * receiveBuffer);
	static int getHeaderSize(char * receiveBuffer);
	
public:
	Param(std::string key, std::string value);
	~Param();

	std::string key;
	std::string value;
	
	void print();
	void printHeader();

	static bool is_letter(char character);
	static bool is_number(char character);
	
	static int getKeysAndValues(char * receiveBuffer, std::vector<Param *> & params);
	static int getHeader(char * receiveBuffer, std::vector<Param *> & params);
};

/*
class Param
{
private:
	unsigned int paramSize;
	
	enum ASCII { STR_ZERO = 48, STR_NINE = 57, UNDER_SCORE = 95, LOWER_LETTER_A = 97, LOWER_LETTER_Z = 122 };
	
	bool is_letter(char character);
	bool is_number(char character);
	int getParamSize(char * receiveBuffer);
	int getHeaderSize(char * receiveBuffer);
	
public:
	Param(unsigned int paramSize);
	~Param();

	std::vector<char *> keys;
	std::vector<char *> values;
	
	std::vector<std::string> keyz;
	std::vector<std::string> valuez;
	
	void print();
	void printHeader();
	
	int getKeyAndValue(char * receiveBuffer);
	int getHeader(char * receiveBuffer);
};
*/

#endif // PARAM_H