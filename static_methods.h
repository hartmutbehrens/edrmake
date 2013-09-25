/*
 * static_methods.h
 *
 *  Created on: 04 Apr 2013
 *      Author: hartmut.behrens
 */

#ifndef __STATIC_METHODS_H_
#define __STATIC_METHODS_H_

class static_methods
{
public:
	static std::string pad_number(int number, int length);
	static std::string pad_number_random(int number, int length);
	static void fillstring(std::string &to_fill, char fill_char, int len);
};

#endif /* STATIC_METHODS_H_ */
