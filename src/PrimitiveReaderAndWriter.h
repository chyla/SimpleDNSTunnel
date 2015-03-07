/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef _PRIMITIVEREADERANDWRITER_H_
#define _PRIMITIVEREADERANDWRITER_H_

#include <mutex>

#include "Interfaces/TunTap.h"
#include "Interfaces/Socket.h"
#include "Packets/Packet.h"


class PrimitiveReaderAndWriter
{
public:
  PrimitiveReaderAndWriter(std::shared_ptr<Interfaces::TunTap> &tuntap,
			   std::shared_ptr<Interfaces::Socket> &socket,
                           std::shared_ptr<Packets::Packet> &prototype);
  virtual ~PrimitiveReaderAndWriter() = default;

  virtual void Run();
  virtual void Stop();

protected:
  std::shared_ptr<Interfaces::TunTap> tuntap;
  std::shared_ptr<Interfaces::Socket> socket;
  std::shared_ptr<Packets::Packet> prototype;
  std::mutex prototype_mutex;

  bool running;

  // these functions don't work in all cases
  void ReadFromTunAndWriteToSocket();
  void ReadFromSocketAndWriteToTun();

  std::unique_ptr<Packets::Packet> ClonePrototype();
};


#endif // _PRIMITIVEREADERANDWRITER_H_
