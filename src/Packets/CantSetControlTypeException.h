/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

namespace Packets
{

class CantSetControlTypeException : public std::domain_error
{
public:
 CantSetControlTypeException() :
   std::domain_error("Can't set control type.")
  {
  }
};

}
