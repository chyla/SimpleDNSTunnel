/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

#ifndef _BADPARTSIZEEXCEPTION_H_
#define _BADPARTSIZEEXCEPTION_H_


namespace Packets
{

class BadPartSizeException : public std::logic_error
{
public:
  BadPartSizeException() :
    std::logic_error("Bad part size.")
  {
  }
};

}

#endif
