#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include "typedefs.h"

#include <iostream>
#include <fstream>
#include <cstddef>
#include <exception>

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

namespace po = boost::program_options;
namespace bf = boost::filesystem;

po::variables_map parse_config(int argc, char* argv[]);

class Config
{
public:
  
  static Config& Instance();

  po::variables_map* GetConfig();

private:
  Config();

  static po::variables_map* config_map_;
  static bool config_valid_;
};

void ReadConfiguration(int argc, char* arvg[]);

void ParseCommandline(int argc, char* argv[]);

void ParseConfigFile(std::string config_file);

void FindSdfFiles();

#endif
