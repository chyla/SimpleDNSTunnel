/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <boost/test/unit_test.hpp>

#include "../src/Options/ProgramOptions.h"
#include "../src/Options/BadOptionValueException.h"

using namespace Options;


BOOST_AUTO_TEST_SUITE( ProgramOptions_ConfigFile )

BOOST_AUTO_TEST_CASE( CommandLine_DefaultValues )
{
  ProgramOptions options;
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::SERVER);
  BOOST_CHECK_EQUAL(options.GetAddress(), "127.0.0.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 53);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( CommandLine_ClientMode )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--mode", "client"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::CLIENT);
  BOOST_CHECK_EQUAL(options.GetAddress(), "127.0.0.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 53);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( CommandLine_ServerMode )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--mode", "server"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::SERVER);
  BOOST_CHECK_EQUAL(options.GetAddress(), "127.0.0.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 53);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( CommandLine_BadMode )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--mode", "bad_value"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);

  BOOST_CHECK_THROW(options.Parse(), BadOptionValueException);
}


BOOST_AUTO_TEST_CASE( CommandLine_ServerModePort )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--port", "5353"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::SERVER);
  BOOST_CHECK_EQUAL(options.GetAddress(), "127.0.0.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 5353);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( CommandLine_ClientModePort )
{
  int argc = 5;
  const char *argv[] = {"program_name", "--mode", "client", "--port", "5353"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::CLIENT);
  BOOST_CHECK_EQUAL(options.GetAddress(), "127.0.0.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 5353);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( CommandLine_PortTooHigh )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--port", "65536"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);

  BOOST_CHECK_THROW(options.Parse(), BadOptionValueException);
}


BOOST_AUTO_TEST_CASE( CommandLine_ServerModeAddress )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--address", "192.168.122.1"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::SERVER);
  BOOST_CHECK_EQUAL(options.GetAddress(), "192.168.122.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 53);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( CommandLine_ServerModeBadAddress )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--address", "192.168.12"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);

  BOOST_CHECK_THROW(options.Parse(), BadOptionValueException);
}


BOOST_AUTO_TEST_CASE( CommandLine_ShowHelp_LongOption )
{
  int argc = 2;
  const char *argv[] = {"program_name", "--help"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK_EQUAL(options.GetShowHelp(), true);
}


BOOST_AUTO_TEST_CASE( CommandLine_ShowHelp_ShortOption )
{
  int argc = 2;
  const char *argv[] = {"program_name", "-h"};

  ProgramOptions options;
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK_EQUAL(options.GetShowHelp(), true);
}


BOOST_AUTO_TEST_CASE( ConfigFile_ReadAllOptions )
{
  ProgramOptions options;
  options.SetConfigFilePath("example1.conf");
  options.Parse();
  
  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::CLIENT);
  BOOST_CHECK_EQUAL(options.GetAddress(), "192.168.122.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 5353);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}


BOOST_AUTO_TEST_CASE( ConfigFile_And_CommandLine )
{
  int argc = 3;
  const char *argv[] = {"program_name", "--mode", "server"};

  ProgramOptions options;
  options.SetConfigFilePath("example1.conf");
  options.SetCommandLineOptions(argc, argv);
  options.Parse();

  BOOST_CHECK(options.GetMode() == ProgramOptions::Mode::SERVER);
  BOOST_CHECK_EQUAL(options.GetAddress(), "192.168.122.1");
  BOOST_CHECK_EQUAL(options.GetPort(), 5353);
  BOOST_CHECK_EQUAL(options.GetShowHelp(), false);
}

BOOST_AUTO_TEST_SUITE_END()
