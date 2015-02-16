/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <cstdint>
#include <vector>

#include "Packet.h"

#ifndef _PSEUDODNS_H_
#define _PSEUDODNS_H_


namespace Packets
{

class PseudoDNS : public Packet
{
public:
  static constexpr int MAX_DATA_SIZE = 255;

  PseudoDNS(const Type &type = Type::DATA);
  virtual ~PseudoDNS() = default;

  virtual void SetType(const Type &type);
  virtual Type GetType() const;

  virtual void SetControlType(const Control &control);
  virtual Control GetControlType() const;

  virtual void SetData(const Data &data);
  virtual Data GetData() const;

  virtual void FillFromDump(const Data &dump);
  virtual Data Dump() const;

private:
  Type type;
  Control control_type;
  Data data;
};

}

#endif
