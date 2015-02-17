/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

#ifndef _TOOMUCHDATAEXCEPTION_H_
#define _TOOMUCHDATAEXCEPTION_H_


namespace Packets
{

class TooMuchDataException : public std::length_error
{
public:
 TooMuchDataException() :
   std::length_error("Size of data buffer exceeds maximum size.")
  {
  }
};

}

#endif
