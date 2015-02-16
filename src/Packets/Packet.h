/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <cstdint>
#include <vector>
#include <memory>

#ifndef _PACKET_H_
#define _PACKET_H_

#ifndef _PACKET_H_
#define _PACKET_H_


namespace Packets
{

class Packet
{
public:
  typedef std::vector<std::uint8_t> Data;

  enum class Type : bool
  {
    DATA,
    CONTROL
  };

  enum class Control : std::uint8_t
  {
    NONE,
    RECEIVED,
    END_OF_TRANSMISSION
  };

  virtual ~Packet() = default;

  virtual void SetType(const Type &type) = 0;
  virtual Type GetType() const = 0;

  virtual void SetControlType(const Control &control) = 0;
  virtual Control GetControlType() const = 0;

  virtual void SetData(const Data &data) = 0;
  virtual Data GetData() const = 0;

  virtual void FillFromDump(const Data &dump) = 0;
  virtual Data Dump() const = 0;

  virtual int GetMaximumDataSize() const = 0;

  virtual std::unique_ptr<Packet> Clone() const = 0;
};

}

#endif
