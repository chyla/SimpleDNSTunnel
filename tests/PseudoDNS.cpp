/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <boost/test/unit_test.hpp>
#include <algorithm>

#include "../src/Packets/PseudoDNS.h"
#include "../src/Packets/TooMuchDataException.h"
#include "../src/Packets/CantSetControlTypeException.h"
#include "../src/Packets/CorruptedPacketException.h"
#include "../src/Packets/WrongMagicNumberException.h"

using namespace Packets;


BOOST_AUTO_TEST_SUITE( PseudoDNS_Tests )

BOOST_AUTO_TEST_CASE( Constructor )
{
  PseudoDNS packet(Packet::Type::DATA);
  PseudoDNS packet2(Packet::Type::CONTROL);
  
  BOOST_CHECK(packet.GetType() == Packet::Type::DATA);
  BOOST_CHECK(packet2.GetType() == Packet::Type::CONTROL);
}


BOOST_AUTO_TEST_CASE( ControlPacket_Clone )
{
  Packet::Data data { 0x11 };
  PseudoDNS packet(Packet::Type::CONTROL);
  packet.SetData(data);
  packet.SetControlType(Packet::Control::END_OF_TRANSMISSION);

  std::unique_ptr<Packet> p = packet.Clone();

  BOOST_CHECK(p->GetType() == packet.GetType());
  BOOST_CHECK(p->GetControlType() == packet.GetControlType());

  Packet::Data new_data = p->GetData();
  BOOST_CHECK_EQUAL_COLLECTIONS(new_data.begin(), new_data.end(),
				data.begin(), data.end());
}


BOOST_AUTO_TEST_CASE( DataPacket_ThrowOnSetControlType )
{
  PseudoDNS packet(Packet::Type::DATA);

  BOOST_CHECK_THROW(packet.SetControlType(Packet::Control::RECEIVED), CantSetControlTypeException);
}


BOOST_AUTO_TEST_CASE( DataPacket_ThrowOnSetTooMuchData )
{
  constexpr char any_data = 0xFA;

  Packet::Data data(PseudoDNS::MAX_DATA_SIZE + 1, any_data);
  PseudoDNS packet(Packet::Type::DATA);

  BOOST_CHECK_THROW(packet.SetData(data), TooMuchDataException);
}


BOOST_AUTO_TEST_CASE( ControlPacket_TypeSet )
{
  PseudoDNS packet(Packet::Type::CONTROL);

  packet.SetControlType(Packet::Control::END_OF_TRANSMISSION);

  BOOST_CHECK(packet.GetControlType() == Packet::Control::END_OF_TRANSMISSION);
}


BOOST_AUTO_TEST_CASE( DataPacket_Dump )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data data(6, any_data);

  PseudoDNS packet(Packet::Type::DATA);
  packet.SetData(data);

  Packet::Data dumped_packet = packet.Dump();

  Packet::Data expected_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(dumped_packet.begin(), dumped_packet.end(),
				expected_dump.begin(), expected_dump.end());
}


BOOST_AUTO_TEST_CASE( DataPacket_DumpZeroLengthData )
{
  Packet::Data data;

  PseudoDNS packet(Packet::Type::DATA);
  packet.SetData(data);

  Packet::Data dumped_packet = packet.Dump();

  Packet::Data expected_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(dumped_packet.begin(), dumped_packet.end(),
				expected_dump.begin(), expected_dump.end());
}


BOOST_AUTO_TEST_CASE( DataPacket_DumpNoData )
{
  PseudoDNS packet(Packet::Type::DATA);

  Packet::Data dumped_packet = packet.Dump();

  Packet::Data expected_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(dumped_packet.begin(), dumped_packet.end(),
				expected_dump.begin(), expected_dump.end());
}


BOOST_AUTO_TEST_CASE( ControlPacket_Dump )
{
  PseudoDNS packet(Packet::Type::CONTROL);
  packet.SetControlType(Packet::Control::END_OF_TRANSMISSION);

  Packet::Data dumped_packet = packet.Dump();

  Packet::Data expected_dump {
    0x14, 0x1D,          // Magic number
    0x01, 0x02,          // DC = 1 (Control), Control type = 2 (END_OF_TRANSMISSION)
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  BOOST_CHECK_EQUAL_COLLECTIONS(dumped_packet.begin(), dumped_packet.end(),
				expected_dump.begin(), expected_dump.end());
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorrectControlPacket )
{
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x01, 0x02,          // DC = 1 (Control), Control type = 2 (END_OF_TRANSMISSION)
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  packet.FillFromDump(packet_dump);

  BOOST_CHECK(packet.GetType() == Packet::Type::CONTROL);
  BOOST_CHECK(packet.GetControlType() == Packet::Control::END_OF_TRANSMISSION);
  BOOST_CHECK_EQUAL(packet.GetData().size(), 0);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorrectDataPacket )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };
  Packet::Data expected_data(6, any_data);

  PseudoDNS packet;
  packet.FillFromDump(packet_dump);

  Packet::Data data = packet.GetData();

  BOOST_CHECK(packet.GetType() == Packet::Type::DATA);
  BOOST_CHECK(packet.GetControlType() == Packet::Control::NONE);
  BOOST_CHECK_EQUAL_COLLECTIONS(expected_data.begin(), expected_data.end(),
				data.begin(), data.end());
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros1 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x10,          // Bad
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros2 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x10, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros3 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x03,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros4 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x10, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros5 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x02, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros6 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0xF0,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonZeros7 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0xF0, 0x00,          // DC = 0, Control type = 0
    0x10, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonOne )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x02,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonOne2 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x02,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_NonOne3 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x02
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongMagicNumber_FirstByte )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x34, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), WrongMagicNumberException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongMagicNumber_SecondByte )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1E,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), WrongMagicNumberException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongControlTypeForDataPacket )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x01,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_DataSizeTooHigh )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_DataSizeTooLow )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x01,                // Data length
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongPacketSize )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
      // 0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
    0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongPacketSize2 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
      // 0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongPacketSize3 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14, 0x1D,          // Magic number
    0x00, 0x00,          // DC = 0, Control type = 0
    0x00, 0x01,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
      // 0x06,                // Data length
    any_data, any_data,
    any_data, any_data,
    any_data, any_data,
    0x00,                // zeros
      // 0x00, 0x01,
    0x00, 0x01
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}


BOOST_AUTO_TEST_CASE( FillPacketFromDump_CorruptedPacket_WrongPacketSize4 )
{
  constexpr unsigned char any_data = 0xFA;
  Packet::Data packet_dump {
    0x14,
  };

  PseudoDNS packet;
  BOOST_CHECK_THROW(packet.FillFromDump(packet_dump), CorruptedPacketException);
}

BOOST_AUTO_TEST_SUITE_END()
