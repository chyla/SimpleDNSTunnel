/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <cstddef>

#ifndef _INTERFACES_H_
#define _INTERFACES_H_


namespace Interfaces
{

class Interface
{
public:
  virtual ~Interface() = default;

  virtual size_t Read(void *destination, const size_t &bufferLength) = 0;
  virtual void Write(const void *source, const size_t &bufferLength) = 0;

  virtual bool IsReadyToRead() const = 0;
};

}

#endif
