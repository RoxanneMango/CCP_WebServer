#ifndef PARAM_H
#define PARAM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include <vector>

class Param
{
private:
	unsigned int paramSize;
	
	enum ASCII { STR_ZERO = 48, STR_NINE = 57, UNDER_SCORE = 95, LOWER_LETTER_A = 97, LOWER_LETTER_Z = 122 };
	
	bool is_letter(char character);
	bool is_number(char character);
	int getParamSize(char * receiveBuffer);
	
public:
	Param(unsigned int paramSize);
	~Param();

	std::vector<char *> keys;
	std::vector<char *> values;

	void print();
	int getKeyAndValue(char * receiveBuffer);
};

#endif // PARAM_H