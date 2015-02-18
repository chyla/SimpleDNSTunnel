/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <iostream>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/trivial.hpp>

#include "Options/ProgramOptions.h"
#include "Interfaces/TunTap.h"
#include "Interfaces/Socket.h"
#include "Packets/PseudoDNS.h"
#include "PrimitiveReaderAndWriter.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;
using namespace Interfaces;
using namespace Packets;

namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;


int
main(int argc, char *argv[])
{
  try {
    boost::log::add_file_log(
      keywords::file_name = LOGDIR "/sdnst.log",
      keywords::format = ( expr::stream << "<"
                                        << boost::log::trivial::severity
                                        << ">\t"
                                        << expr::smessage
			   ),
      keywords::auto_flush = true
    );
    BOOST_LOG_TRIVIAL(info) << "SimpleDNSTunnel";
#ifdef HAVE_CONFIG_H
    BOOST_LOG_TRIVIAL(info) << "Version: " << VERSION;
#endif
    BOOST_LOG_TRIVIAL(info) << "Copyright 2014-2015 Adam Chyła, adam@chyla.org";
    BOOST_LOG_TRIVIAL(info) << "All rights reserved. Distributed under the terms of the MIT License.";

    Options::ProgramOptions options;
    options.SetCommandLineOptions(argc, const_cast<const char**>(argv));
    options.SetConfigFilePath(SYSCONFDIR "/sdnst.conf");
    options.Parse();

    if (options.GetShowHelp()) {
      cout << " Simple DNS Tunnel\n===================\n";
      cout << options.GetHelpMessage() << "\n";
      return 0;
    }

    // create interfaces
    unique_ptr<TunTap> tuntap(TunTap::Create(TunTap::InterfaceType::TUN));
    unique_ptr<Socket> socket(Socket::Create(Socket::DomainType::INET,
					     Socket::SocketType::DGRAM));

    if (options.GetMode() == Options::ProgramOptions::Mode::CLIENT)
      socket->Connect(options.GetAddress(), options.GetPort());
    else
      socket->Bind(options.GetPort(), options.GetAddress());

    // start tunneling
    unique_ptr<Packet> prototype(new PseudoDNS());
    PrimitiveReaderAndWriter rw(tuntap, socket, prototype);
    rw.Run();

  } catch (exception &ex) {
    std::cerr << ex.what() << '\n';
    BOOST_LOG_TRIVIAL(fatal) << ex.what();
    return 1;
  }

  return 0;
}
