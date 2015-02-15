/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "PseudoDNS.h"
#include "TooMuchDataException.h"
#include "CantSetControlTypeException.h"
#include "CorruptedPacketException.h"
#include "WrongMagicNumberException.h"

#include <utility>
#include <algorithm>

using namespace std;


namespace Packets
{

PseudoDNS::PseudoDNS(const Packet::Type &type) :
  type(type),
  control_type(Packet::Control::NONE)
{
}


void
PseudoDNS::SetType(const Packet::Type &type)
{
  this->type = type;
}


Packet::Type
PseudoDNS::GetType() const
{
  return type;
}


void
PseudoDNS::SetControlType(const Control &control)
{
  if (type == Type::DATA)
    throw CantSetControlTypeException();

  control_type = control;
}


Packet::Control
PseudoDNS::GetControlType() const
{
  return control_type;
}


void
PseudoDNS::SetData(const Packet::Data &data)
{
  if (data.size() > MAX_DATA_SIZE)
    throw TooMuchDataException();

  this->data = data;
}


Packet::Data
PseudoDNS::GetData() const
{
  return data;
}


void
PseudoDNS::FillFromDump(const Packet::Data &dump)
{
  if (dump.size() < 17)
    throw CorruptedPacketException();

  // Check magic number
  if (dump.at(0) != 0x14 || dump.at(1) != 0x1D)
    throw WrongMagicNumberException();

  // Check constans
  if (((dump.at(2) & 0xFE) != 0x00)
      || ((dump.at(3) & 0xF0) != 0x00))
    throw CorruptedPacketException();

  vector<pair<int, unsigned char>> position_value {
    {4, 0x00},
    {5, 0x01},
    {6, 0x00},
    {7, 0x00},
    {8, 0x00},
    {9, 0x00},
    {10, 0x00},
    {11, 0x00},
    {dump.size() - 5, 0x00},
    {dump.size() - 4, 0x00},
    {dump.size() - 3, 0x01},
    {dump.size() - 2, 0x00},
    {dump.size() - 1, 0x01}
  };
  for_each(position_value.begin(), position_value.end(),
	   [&dump](pair<int, unsigned char> &p) {
	     if (dump.at(p.first) != p.second)
	       throw CorruptedPacketException();
	   });

  // Check data size
  constexpr int data_position = 13;
  constexpr int data_size_position = data_position - 1;
  const uint8_t data_size = dump.at(data_size_position);

  // Check size of packet
  const int calculated_end_position = data_size_position + data_size + 4  + (data_size != 0);
  const int end_position = dump.size() - 1;
  if (calculated_end_position != end_position)
    throw CorruptedPacketException();

  // Copy values
  type = static_cast<Packet::Type>(dump.at(2) & 0x01);
  control_type = static_cast<Packet::Control>(dump.at(3) & 0x0F);

  if (type == Packet::Type::DATA && control_type != Packet::Control::NONE)
    throw CorruptedPacketException();

  data.insert(data.begin(),
	      dump.begin() + data_position,
	      dump.begin() + data_position + data_size);
}


Packet::Data
PseudoDNS::Dump() const
{
  Data dump(12, 0x00);
  // Magic number
  dump.at(0) = 0x14;
  dump.at(1) = 0x1D;

  // DC - Data or control packet
  dump.at(2) = static_cast<uint8_t>(type);

  // Control type
  dump.at(3) = static_cast<uint8_t>(control_type);

  dump.at(5) = 0x01;

  if (data.size() > 0)
  {
    // Data size
    dump.push_back(static_cast<uint8_t>(data.size()));

    // Data
    dump.insert(dump.end(), data.begin(), data.end());
  }

  // last 5 bytes
  const char bytes[] {
    0x00,
    0x00, 0x01,
    0x00, 0x01
  };

  dump.insert(dump.end(), bytes, bytes + sizeof(bytes));

  return dump;
}

}
