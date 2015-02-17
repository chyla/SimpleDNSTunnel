/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <stdexcept>
#include <string>

#ifndef _CORRUPTEDPACKETEXCEPTION_H_
#define _CORRUPTEDPACKETEXCEPTION_H_

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

#endif
