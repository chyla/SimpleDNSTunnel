/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <iostream>

#include "Options/ProgramOptions.h"

using namespace std;


int
main(int argc, char *argv[])
{
  try {
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
    cerr << ex.what() << '\n';
  }

  return 0;
}
