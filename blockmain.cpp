#include "blockmain.h"

namespace po = boost::program_options;

po::variables_map parse_config(int argc, char* argv[]){
  po::options_description mainopts("Allowed Options");
  mainopts.add_options()
    ("help,h", "Print this help message")
    ("datafile", po::value<std::string>(), "Datafile");

  po::positional_options_description positional;
  positional.add("datafile", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(mainopts).positional(positional).run(), vm);
  po::notify(vm);


  if(vm.count("help")){
    std::cout << mainopts << std::endl;;
  }

  return(vm);
}


int main(int argc, char* argv[]){
  
  po::variables_map config;
  sdf_file_t* sdf_handle;
  int sdf_status;

  config = parse_config(argc, argv);
  if(config.count("help")){
    return(0);
  }
  if(!config.count("datafile")){
    std::cerr << "No sdf file specified!" << std::endl;
    return(-1);
  }

  sdf_handle = sdf_open(config["datafile"].as<std::string>().c_str(), 0, SDF_READ, 1);
  if(!sdf_handle){
    std::cerr << "Error opening " << config["datafile"].as<std::string>() << "." << std::endl;
    return(-1);
  }

  std::cout << "SDF File Opened Sucessfully.\n"
       << "Created by " << sdf_handle->code_name << ".\n"
       << sdf_handle->nblocks << " blocks present.\n\n" << std::flush;

  sdf_read_blocklist(sdf_handle);

  sdf_block_t* curr_block;
  curr_block = sdf_handle->blocklist;
  for(int count=0; count < sdf_handle->nblocks; count++){
    std::cout << "\t* " << curr_block->name << "\n";
    curr_block = curr_block->next;
  }
  std::cout << std::endl;

  return(0);
}
