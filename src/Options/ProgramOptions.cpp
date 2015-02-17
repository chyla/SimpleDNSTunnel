/*
 * Copyright 2014-2015 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <cassert>
#include <boost/regex.hpp>
#include <sstream>
#include <string>
#include <boost/log/trivial.hpp>

#include "ProgramOptions.h"
#include "BadOptionValueException.h"


namespace Options
{

using namespace std;
using namespace boost::program_options;
using namespace boost;


ProgramOptions::ProgramOptions() :
  general_options("General options"),
  help_options("Help options"),
  cmd_params(0, nullptr),
  mode(ProgramOptions::Mode::SERVER),
  address("127.0.0.1"),
  port(53),
  show_help(false)
{
  general_options.add_options()
    ("mode", value<string>(), "server|client\n\
default: server\n")
    ("address", value<string>(), "server: adderss to bind\n\
client: address to connect\n\
default: 127.0.0.1\n")
    ("port", value<unsigned>(), "server: port to listen\n\
client: port to connect\n\
default: 53");

  help_options.add_options()
    ("help,h", "print help message and exit");

  all_options.add(general_options);
  all_options.add(help_options);
}


void
ProgramOptions::SetCommandLineOptions(const int &argc, const char *argv[])
{
  cmd_params = make_pair(argc, argv);
}


void
ProgramOptions::SetConfigFilePath(const string &path)
{
  config_file_path = path;
}


void
ProgramOptions::Parse()
{
  store(parse_command_line(cmd_params.first, cmd_params.second, all_options),
	variables);

  if (!config_file_path.empty()) {
    BOOST_LOG_TRIVIAL(info) << "Reading config file: " << config_file_path;
    OpenConfigFile();
    store(parse_config_file(config_file, general_options),
	  variables);
  }

  notify(variables);

  show_help = variables.count("help");

  if (variables.count("mode"))
    SetMode(variables["mode"].as<string>());

  if (variables.count("address"))
    SetIp(variables["address"].as<string>());

  if (variables.count("port"))
    SetPort(variables["port"].as<unsigned>());
}


string
ProgramOptions::GetHelpMessage() const
{
  stringstream stream;
  stream << all_options;

  return stream.str();
}


ProgramOptions::Mode
ProgramOptions::GetMode() const
{
  return mode;
}


string
ProgramOptions::GetAddress() const
{
  return address;
}


int
ProgramOptions::GetPort() const
{
  return port;
}


bool
ProgramOptions::GetShowHelp() const
{
  return show_help;
}


void
ProgramOptions::OpenConfigFile()
{
  config_file.open(config_file_path.c_str());
  if (!config_file)
    throw runtime_error("cannot open config file: " + config_file_path);
}


void
ProgramOptions::SetMode(const std::string &mode)
{
  if (mode == "client")
    this->mode = Mode::CLIENT;
  else if (mode == "server")
    this->mode = Mode::SERVER;
  else
    throw BadOptionValueException("mode", mode);
}


void
ProgramOptions::SetIp(const std::string &address)
{
  regex ip("(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");

  if (!regex_match(address, ip))
    throw BadOptionValueException("address", address);

  this->address = address;
}


void
ProgramOptions::SetPort(const unsigned &port)
{
  if (port > 65535)
    throw BadOptionValueException("port", to_string(port));

  this->port = port;
}

}
