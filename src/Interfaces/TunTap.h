/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <boost/noncopyable.hpp>
#include <memory>
//#include <string>

#include "Interface.h"

#ifndef _TUNTAP_H_
#define _TUNTAP_H_


namespace Interfaces
{

class TunTap : public Interface, private boost::noncopyable
{
public:
  enum class InterfaceType
  {
    TUN,
    TAP
  };

  virtual ~TunTap();

  static std::unique_ptr<TunTap> Create(InterfaceType type);

  InterfaceType GetType() const;
  std::string GetName() const;

  size_t Read(void *destination, const size_t &bufferLength);
  void Write(const void *source, const size_t &bufferLength);

  void Close();

private:
  TunTap(InterfaceType type);

  InterfaceType type;
  std::string name;
  int fd;
  bool close_executed;
};

}

#endif
