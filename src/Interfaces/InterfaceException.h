/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

#ifndef _INTERFACEEXCEPTION_H_
#define _INTERFACEEXCEPTION_H_


namespace Interfaces
{

class InterfaceException : public std::runtime_error
{
public:
 InterfaceException(const std::string &message) :
   std::runtime_error(message)
  {
  }
};

}

#endif
