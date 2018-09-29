#include "configuration.h"

bool Config::config_valid_ = false;
po::variables_map* Config::config_map_ = NULL;

Config& Config::Instance(){
  static Config* instance = new Config();
  return *instance;
}

Config::Config(){
  config_map_ = new po::variables_map;
}

po::variables_map* Config::GetConfig(){
  return config_map_;
}

void ReadConfiguration(int argc, char* argv[]){
	
  po::variables_map* config = Config::Instance().GetConfig();

	ParseCommandline(argc, argv);

  if (!config->count("configfile")){
    std::cout << "Error, no configfile given" << std::endl;
    throw std::invalid_argument("Non-existent configfile");
  }

  ParseConfigFile((*config)["configfile"].as<std::string>());

  return;

}

void ParseCommandline(int argc, char* argv[]){
  po::options_description visible_options("Allowed Options");
  visible_options.add_options()
    ("help,h", "Print this help message");

  po::options_description hidden_options("Hidden Options");
  hidden_options.add_options()
    ("configfile", po::value<std::string>(), "");

  po::positional_options_description positional;
  positional.add("configfile", 1);

  po::options_description cmdline_options;
  cmdline_options.add(visible_options).add(hidden_options);

  po::variables_map* vm = Config::Instance().GetConfig();
  po::store(po::command_line_parser(argc, argv)
              .options(cmdline_options)
              .positional(positional)
              .run()
					 ,*vm);
  po::notify(*vm);

  if(vm->count("help") || !vm->count("configfile")){
    std::cout << visible_options << std::endl;

  }
}

void ParseConfigFile(std::string config_file){
  po::variables_map* vm = Config::Instance().GetConfig();

  po::options_description mainopts("");
  mainopts.add_options()
    ("gamma_min", po::value<double>()->default_value(10), "")
    ("time_min", po::value<double>()->default_value(0), "")
    ("time_max", po::value<double>()->default_value(std::numeric_limits<double>::max()), "")
    ("rand_frac", po::value<double>()->default_value(2), "")
    ("output_filename", po::value<std::string>()->default_value("output.h5"), "")
    ("species", po::value<std::string>()->default_value("electron"), "")
    ("prefix", po::value<std::string>()->default_value(""), "")
    ("radt_mode", po::value<bool>()->default_value(false), "");


  std::ifstream ifs;
  ifs.exceptions(std::ifstream::failbit | std::fstream::badbit);
  try{
    ifs.open(config_file.c_str(), std::fstream::in);
  }
  catch (std::exception& e){
    std::cout << "Error opening \"" << config_file << "\"" << std::endl << std::endl;
    throw;
  }
  ifs.exceptions(std::ifstream::goodbit);

  try{
    po::store(po::parse_config_file(ifs, mainopts), *vm);
    po::notify(*vm);
  }
  catch(std::exception& e){
    std::cout << "Error occured while reading config file:" <<std::endl;
    std::cout << e.what() << std::endl << std::endl;
    throw;
  }

  return;
}

void FindSdfFiles(){
  po::variables_map* vm = Config::Instance().GetConfig();

  PathVector* sdf_list = new PathVector;

  bf::path localdir(".");

  bf::directory_iterator end_itr;
  for(bf::directory_iterator itr(localdir); itr != end_itr; ++itr){
    if(itr->path().extension() == ".sdf"){
      sdf_list->push_back(itr->path());
    }
  }
  std::sort(sdf_list->begin(),sdf_list->end());

  vm->insert(std::make_pair("sdf_list", po::variable_value(sdf_list, false)));
}

