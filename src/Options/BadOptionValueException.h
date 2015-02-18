/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

#ifndef _BADOPTIONVALUEEXCEPTION_H_
#define _BADOPTIONVALUEEXCEPTION_H_


namespace Options
{

class BadOptionValueException : public std::logic_error
{
public:
 BadOptionValueException(std::string optionName, std::string value) :
    logic_error("bad " + optionName + " value '" + value + "'")
  {
  }
};

}

#endif
