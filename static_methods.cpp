/*
 * static_method.cpp

 *
 *  Created on: 04 Apr 2013
 *      Author: hartmut.behrens
 */
#include <cstdlib>
#include <sstream>
#include "static_methods.h"

std::string static_methods::pad_number(int number, int length)
{
	std::ostringstream oss;
	std::string text;
	oss << number;
	std::string rec_text = oss.str();
	fillstring(text,'0', (int)( length - rec_text.size() ) );
	text += rec_text;
	return text;
}

void static_methods::fillstring(std::string &to_fill, char fill_char, int len)
{
	if (len < 0)
	{
		throw new std::string("fillstring request with a negative length");
	}
	while(to_fill.length() < len)
	{
		to_fill += fill_char;
	}
}

std::string static_methods::make_random_num(int highest)
{
	std::string random_integer;
	std::stringstream out;
	int rnum = (rand () % highest) + 1;
	out << rnum;
	random_integer = out.str();
	return random_integer;
}



