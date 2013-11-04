/*
 * Helper.h
 *
 *  Created on: 04.11.2013
 *      Author: richie
 */

#ifndef HELPER_H_
#define HELPER_H_

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <list>

class Helper {
public:
	static ssize_t readline(int fd, char *vptr, size_t maxlen);
	static std::string removeNewline(std::string s);
	static std::list<std::string> splitString(std::string string,
			std::string delimiter);
private:
	static ssize_t my_read(int fd, char *ptr);

};

#endif /* HELPER_H_ */
