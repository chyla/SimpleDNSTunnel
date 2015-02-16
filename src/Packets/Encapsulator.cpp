/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "Encapsulator.h"
#include "BadPartSizeException.h"

#include <algorithm>

using namespace std;


namespace Packets
{

Encapsulator::Encapsulator(unique_ptr<Packet> &&prototype) :
  prototype(std::move(prototype)),
  part_size(0)
{
}


void
Encapsulator::SetPartSize(size_t part_size)
{
  const size_t max_size = prototype->GetMaximumDataSize();
  if (part_size > max_size)
    throw BadPartSizeException();

  this->part_size = part_size;
}


vector<unique_ptr<Packet>>
Encapsulator::Encapsulate(Packet::Data data) const
{
  const size_t size = (part_size == 0) ? prototype->GetMaximumDataSize() : part_size;
  vector<unique_ptr<Packet>> packets;

  for (size_t i = 0; i < data.size(); i += size)
  {
    Packet::Data part;
    part.insert(part.begin(),
		data.begin() + i,
		data.begin() + min(i + size, data.size()));

    unique_ptr<Packet> p = prototype->Clone();
    p->SetData(part);
    packets.push_back(move(p));
  }

  return packets;
}

}
