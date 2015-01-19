/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <iostream>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/trivial.hpp>

#include "Options/ProgramOptions.h"

using namespace std;
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
      )
    );
    BOOST_LOG_TRIVIAL(info) << "Starting SimpleDNSTunnel";

    Options::ProgramOptions options;
    options.SetCommandLineOptions(argc, const_cast<const char**>(argv));
    options.SetConfigFilePath(SYSCONFDIR "/sdnst.conf");
    options.Parse();

    if (options.GetShowHelp()) {
      cout << " Simple DNS Tunnel\n===================\n";
      cout << options.GetHelpMessage() << "\n";
      return 0;
    }
  } catch (exception &ex) {
    std::cerr << ex.what() << '\n';
    BOOST_LOG_TRIVIAL(fatal) << ex.what();
    return 1;
  }

  return 0;
}
