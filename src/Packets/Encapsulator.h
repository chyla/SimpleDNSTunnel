/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <memory>
#include <vector>

#include "Packet.h"

#ifndef _ENCAPSULATOR_H_
#define _ENCAPSULATOR_H_


namespace Packets
{

class Encapsulator
{
public:
  Encapsulator(std::unique_ptr<Packet> &&prototype);
  virtual ~Encapsulator() = default;

  virtual void SetPartSize(size_t part_size);
  virtual std::vector<std::unique_ptr<Packet>> Encapsulate(const Packet::Data &data) const;
  virtual Packet::Data Decapsulate(const std::vector<std::unique_ptr<Packet>> &packets) const;

protected:
  std::unique_ptr<Packet> prototype;
  size_t part_size;
};

}

#endif
