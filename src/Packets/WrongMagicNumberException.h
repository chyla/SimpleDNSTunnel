/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

namespace Packets
{

class WrongMagicNumberException : public std::runtime_error
{
public:
 WrongMagicNumberException() :
    std::runtime_error("Wrong magic number.")
  {
  }
};

}
