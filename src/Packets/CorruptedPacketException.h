/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

namespace Packets
{

class CorruptedPacketException : public std::runtime_error
{
public:
 CorruptedPacketException() :
    std::runtime_error("Packet is corrupted.")
  {
  }
};

}
