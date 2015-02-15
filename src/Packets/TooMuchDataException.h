/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

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
