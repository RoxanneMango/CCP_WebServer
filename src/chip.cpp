#include "chip.h"

Chip::Chip(double amount)
{
	try
	{
		if(isValid(amount))
		{
			this->amount = amount;
		}
		else
		{
			throw "The amount was not correct ; destroying object again . . .";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
		this->Chip::~Chip();
	}
}

Chip::Chip()
{}

Chip::~Chip()
{}

bool
Chip::isValid(double amount)
{
	if(amount == 1.00)
	{
		color = "White";
		return true;
	}
	else if(amount == 2.50)
	{
		color = "Pink";
		return true;
	}
	else if(amount == 5.00)
	{
		color = "Red";
		return true;
	}
	else if(amount == 10.00)
	{
		color = "Blue";
		return true;
	}
	else if(amount == 20.00)
	{
		color = "Gray";
		return true;
	}
	else if(amount == 25.00)
	{
		color = "Green";
		return true;
	}
	else if(amount == 50.00)
	{
		color = "Orange";
		return true;
	}
	else if(amount == 100.00)
	{
		color = "Black";
		return true;
	}
	else if(amount == 500.00)
	{
		color = "Purple";
		return true;
	}
	else if(amount == 1000.00)
	{
		color = "Yellow";
		return true;
	}
	else if(amount == 5000.00)
	{
		color = "Brown";
		return true;
	}
	
	return false;
}

void
Chip::set(double amount)
{
	if(isValid(amount))
	{
		this->amount = amount;
	}
}

double
Chip::getAmount()
{
	return amount;
}

std::string
Chip::getColor()
{
	return color;
}

bool Chip::operator == (Chip & chip)
{
	if(amount == chip.getAmount())
	{
		if(color == chip.getColor())
		{
			return true;
		}
	}
	return false;
}