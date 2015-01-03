/*
 * Copyright 2014 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef _PROGRAM_OPTIONS_
#define _PROGRAM_OPTIONS_

#include <boost/program_options.hpp>
#include <fstream>
#include <string>
#include <utility>


namespace Options
{

class ProgramOptions
{
public:
  enum class Mode
  {
    SERVER,
    CLIENT
  };

  ProgramOptions();
  virtual ~ProgramOptions() = default;

  void SetCommandLineOptions(const int &argc, const char *argv[]);
  void SetConfigFilePath(const std::string &path);

  void Parse();

  std::string GetHelpMessage() const;

  Mode GetMode() const;
  std::string GetAddress() const;
  int GetPort() const;
  bool GetShowHelp() const;

private:
  typedef boost::program_options::options_description options_description;
  typedef boost::program_options::variables_map variables_map;

  options_description general_options,
                      help_options,
                      all_options;

  variables_map variables;

  std::pair<int, const char **> cmd_params;
  std::string config_file_path;
  std::ifstream config_file;

  Mode mode;
  std::string address;
  unsigned port;
  bool show_help;

  void OpenConfigFile();
  void SetMode(const std::string &mode);
  void SetIp(const std::string &address);
  void SetPort(const unsigned &port);
};

}

#endif // _PROGRAM_OPTIONS_
