/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "PrimitiveReaderAndWriter.h"

#include <thread>
#include <stdexcept>
#include <boost/log/trivial.hpp>
#include <unistd.h>

#include "Packets/Packet.h"
#include "Packets/Encapsulator.h"

using namespace std;
using namespace Interfaces;
using namespace Packets;


PrimitiveReaderAndWriter::PrimitiveReaderAndWriter(unique_ptr<TunTap> &tuntap,
                                                   unique_ptr<Socket> &socket,
                                                   unique_ptr<Packet> &prototype)
 :
  tuntap(move(tuntap)),
  socket(move(socket)),
  prototype(move(prototype)),
  running(false)
{
}


void
PrimitiveReaderAndWriter::Run()
{
  BOOST_LOG_TRIVIAL(info) << "Starting sending/receiving threads...";
  running = true;

  thread t1(&PrimitiveReaderAndWriter::ReadFromTunAndWriteToSocket, this);
  thread t2(&PrimitiveReaderAndWriter::ReadFromSocketAndWriteToTun, this);

  t1.join();
  t2.join();
}


void
PrimitiveReaderAndWriter::ReadFromTunAndWriteToSocket() try
{
  Encapsulator encapsulator(ClonePrototype());
  Packet::Data data;

  while (running)
  {
    if (!socket->IsConnected())
    {
      usleep(500);
      continue;
    }

    data.resize(150);
    const int r = tuntap->Read(data.data(), data.size());
    data.resize(r);

    auto packets = encapsulator.Encapsulate(data);

    auto last = ClonePrototype();
    last->SetType(Packet::Type::CONTROL);
    last->SetControlType(Packet::Control::END_OF_TRANSMISSION);
    packets.push_back(move(last));

    for (auto &packet : packets)
    {
      auto dump = packet->Dump();
      socket->Write(dump.data(), dump.size());
    }
  }
}
catch (exception &ex) {
  BOOST_LOG_TRIVIAL(fatal) << ex.what();
  running = false;
}


void
PrimitiveReaderAndWriter::ReadFromSocketAndWriteToTun() try
{
  Encapsulator encapsulator(ClonePrototype());
  vector<unique_ptr<Packet>> received_packets;
  Packet::Data dump;
  string address;
  int port;

  while (running)
  {
    dump.resize(150);
    const int r = socket->RecvFrom(dump.data(), dump.size(), address, port);
    dump.resize(r);

    if (!socket->IsConnected())
      socket->Connect(address, port);

    auto packet = ClonePrototype();
    packet->FillFromDump(dump);

    if (packet->GetType() == Packet::Type::CONTROL)
    {
      Packet::Data data = encapsulator.Decapsulate(received_packets);
      tuntap->Write(data.data(), data.size());
      received_packets.clear();
    }
    else
      received_packets.push_back(move(packet));
  }
}
catch (exception &ex) {
  BOOST_LOG_TRIVIAL(fatal) << ex.what();
  running = false;
}


unique_ptr<Packet>
PrimitiveReaderAndWriter::ClonePrototype()
{
  lock_guard<mutex> lock(prototype_mutex);
  return prototype->Clone();
}
