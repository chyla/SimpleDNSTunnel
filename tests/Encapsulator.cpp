/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <memory>
#include <vector>

#include "../src/Packets/Packet.h"
#include "../src/Packets/Encapsulator.h"
#include "../src/Packets/BadPartSizeException.h"

using namespace Packets;


class RawDataTests : public Packet
{
public:
  ~RawDataTests() = default;

  virtual void SetType(const Type &type)
  {
    throw std::logic_error("RawDataTests::SetType not implemented.");
  }

  virtual Type GetType() const
  {
    throw std::logic_error("RawDataTests::GetType not implemented.");
  }


  virtual void SetControlType(const Control &control)
  {
    throw std::logic_error("RawDataTests::SetControlType not implemented.");
  }

  virtual Control GetControlType() const
  {
    throw std::logic_error("RawDataTests::GetControlType not implemented.");
  }


  virtual void SetData(const Data &data)
  {
    this->data = data;
  }

  virtual Data GetData() const
  {
    return data;
  }

  virtual void FillFromDump(const Data &dump)
  {
    throw std::logic_error("RawDataTests::FillFromDump not implemented.");
  }

  virtual Data Dump() const
  {
    throw std::logic_error("RawDataTests::Dump not implemented.");
  }

  virtual int GetMaximumDataSize() const
  {
    return 3;
  }

  virtual std::unique_ptr<Packet> Clone() const
  {
    std::unique_ptr<Packet> p(new RawDataTests());

    p->SetData(data);

    return p;
  }

private:
  Packet::Data data;
};


BOOST_AUTO_TEST_SUITE( Encapsulator_Tests )

BOOST_AUTO_TEST_CASE( Encapsulate_PartSizeFromPacketObject )
{
  std::unique_ptr<Packet> prototype(new RawDataTests());
  Packet::Data data { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

  Encapsulator enc(std::move(prototype));
  std::vector<std::unique_ptr<Packet>> packets = enc.Encapsulate(data);

  std::vector<Packet::Data> expected {
    { 0x00, 0x01, 0x02 },
    { 0x03, 0x04, 0x05 },
    { 0x06, 0x07, 0x08 },
    { 0x09 }
  };

  BOOST_REQUIRE_EQUAL(expected.size(), packets.size());
  for (unsigned i = 0; i < expected.size(); i++)
  {
    auto packet_data = packets[i]->GetData();
    BOOST_CHECK_EQUAL_COLLECTIONS(expected[i].begin(), expected[i].end(),
				  packet_data.begin(), packet_data.end());
  }
}


BOOST_AUTO_TEST_CASE( Encapsulate_PartSizeFromUser )
{
  std::unique_ptr<Packet> prototype(new RawDataTests());
  Packet::Data data { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

  Encapsulator enc(std::move(prototype));
  enc.SetPartSize(2);
  std::vector<std::unique_ptr<Packet>> packets = enc.Encapsulate(data);

  std::vector<Packet::Data> expected {
    { 0x00, 0x01 },
    { 0x02, 0x03 },
    { 0x04, 0x05 },
    { 0x06, 0x07 },
    { 0x08, 0x09 }
  };

  BOOST_REQUIRE_EQUAL(expected.size(), packets.size());
  for (unsigned i = 0; i < expected.size(); i++)
  {
    auto packet_data = packets[i]->GetData();
    BOOST_CHECK_EQUAL_COLLECTIONS(expected[i].begin(), expected[i].end(),
				  packet_data.begin(), packet_data.end());
  }
}


BOOST_AUTO_TEST_CASE( Encapsulate_PartSizeFromUser_ThrowWhenBiggerThenMaxPacketSize )
{
  std::unique_ptr<Packet> prototype(new RawDataTests());

  Encapsulator enc(std::move(prototype));

  BOOST_CHECK_THROW(enc.SetPartSize(4), BadPartSizeException);
}


BOOST_AUTO_TEST_CASE( Encapsulator_Decapsulate )
{
  std::unique_ptr<Packet> prototype(new RawDataTests());
  std::vector<std::unique_ptr<Packet>> packets;

  packets.push_back(prototype->Clone());
  packets.push_back(prototype->Clone());
  packets.push_back(prototype->Clone());

  Packet::Data source[3] {
    { 0x00, 0x01, 0x02 },
    { 0x03, 0x04, 0x05 },
    { 0x06, 0x07 }
  };

  packets[0]->SetData(source[0]);
  packets[1]->SetData(source[1]);
  packets[2]->SetData(source[2]);

  Encapsulator enc(std::move(prototype));
  Packet::Data data = enc.Decapsulate(packets);

  Packet::Data expected_data { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
  BOOST_CHECK_EQUAL_COLLECTIONS(data.begin(), data.end(),
				expected_data.begin(), expected_data.end());
}

BOOST_AUTO_TEST_SUITE_END()
