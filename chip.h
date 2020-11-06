#ifndef CHIP_H
#define CHIP_H

#include <string>

class Chip
{
private:
	double amount = 0;
	std::string color = "White";
	
	bool isValid(double amount);
	void setColor();
	
public:
	Chip();
	~Chip();

	Chip(double amount);
	
	void set(double amount);
	double getAmount();
	std::string getColor();
	
	bool operator == (Chip & chip);
	
};

#endif // CHIP_H