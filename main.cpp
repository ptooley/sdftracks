#include "main.h"

int main(int argc, char* argv[]){

  namespace bf = boost::filesystem;
  try{
    ReadConfiguration(argc, argv);
  }
  catch(std::exception& e){
    std::cout << "Fatal Error: Failed to get complete configuration. Quitting" << std::endl;
    return(-1);
  }

  po::variables_map* config = Config::Instance().GetConfig();

  bf::path localdir(".");

  std::string hdf5out = config->at("output_filename").as<std::string>();
  std::string species = config->at("species").as<std::string>();
  double gamma_thres = config->at("gamma_min").as<double>();
  double time_min = config->at("time_min").as<double>();
  double time_max = config->at("time_max").as<double>();
  double rand_frac = config->at("rand_frac").as<double>();
  bool radt_mode = config->at("radt_mode").as<bool>();

  FindSdfFiles();
  PathVector* sdf_list = config->at("sdf_list").as<PathVector*>();

  sdf_file_t* sdf_handle = NULL;
  const int mpi_comm = 0;
  const int mmap = 1;

  int64_t nelem;

  int error;
  
  int count_freq = 1;
  int pcount_freq = 1000;

  int pos_dims = 0;
  int mom_dims = 0;
  sdf_block_t* target_block, *id_block, *weight_block;

  std::string pos_block_name;
  StringVector pos_dimensions;
  sdf_block_t* pos_block;

  sdf_block_t* mom_blocks[3];
  StringVector mom_block_names;

  Int64Vector selected_ids;

  void (*ins_pos[4]) (Particle& part, sdf_block_t* block, int64_t i);
  void (*ins_mom[4]) (Particle& part, sdf_block_t** block, int64_t i, int64_t len);
  
  if (radt_mode){
    ins_pos[1] = radt_1pos;
    ins_pos[2] = radt_2pos;
    ins_pos[3] = radt_3pos;
    ins_mom[1] = radt_1mom;
    ins_mom[2] = radt_2mom;
    ins_mom[3] = radt_3mom;
  }else{
    ins_pos[1] = std_1pos;
    ins_pos[2] = std_2pos;
    ins_pos[3] = std_3pos;
    ins_mom[1] = std_1mom;
    ins_mom[2] = std_2mom;
    ins_mom[3] = std_3mom;
  }

  if(sdf_list->size() < 1){
    std::cerr << "No sdf files found. Quitting." << std::endl;
    return(1);
  }


  for(auto file_iter = sdf_list->begin(); file_iter != sdf_list->end(); file_iter++){
    sdf_handle = sdf_open(file_iter->string().c_str(), mpi_comm, SDF_READ, mmap);
    if(!sdf_handle) continue;

    error = sdf_read_blocklist(sdf_handle);
    if(error){
      sdf_close(sdf_handle);
      continue;
    }

    if(sdf_handle->nblocks > 3) break;
  }

  if(!sdf_handle){
    std::cerr << "Error: No valid SDF files found." << std::endl;
    return(1);
  }


  std::cout << "Checking available data (using " << sdf_handle->filename
            << ")." << std::endl;

  //First check for an ID block, pointless to continue without.
  id_block = sdf_find_block_by_name(sdf_handle, ("Particles/ID/" + species).c_str());
  if(!id_block){
    std::cerr << "Error: Particles/ID/" + species << " not present. Pointless to continue..."
              << std::endl;
    return(1);
  }
  //now check for a weight block, again, pointless to continue if this is missing.
  weight_block = sdf_find_block_by_name(sdf_handle, ("Particles/Weight/" + species).c_str());
  if(!weight_block){
    std::cerr << "Error: Particles/Weight/" + species << " not present. Pointless to continue..."
              << std::endl;
    return(1);
  }

  //Now try to find position data
  target_block = sdf_find_block_by_name(sdf_handle, ("Grid/Particles/" + species).c_str());
  if(target_block){
    pos_dims = target_block->ndims;
    pos_block_name = target_block->name;
    std::cout << target_block->blocktype << std::endl;
    std::cout << "Found position data: (";
    for(int i=0; i < pos_dims;i++){
      pos_dimensions.push_back(std::string(target_block->dim_labels[i]));
      std::cout << target_block->dim_labels[i];
      if(i < (pos_dims - 1)){
        std::cout << ", ";
      }
    }
    std::cout << ")" << std::endl;
  } else {
    std::cerr << "Position information not present, will not continue" << std::endl;
    return(-1);
  }


  //Now try to find momentum direction data
  target_block = sdf_find_block_by_name(sdf_handle, ("Particles/Px/" + species).c_str());
  if(target_block){
    mom_block_names.push_back(std::string(target_block->name));
    mom_dims++;
  }
  target_block = sdf_find_block_by_name(sdf_handle, ("Particles/Py/" + species).c_str());
  if(target_block){
    mom_block_names.push_back(std::string(target_block->name));
    mom_dims++;
  }
  target_block = sdf_find_block_by_name(sdf_handle, ("Particles/Pz/" + species).c_str());
  if(target_block){
    mom_block_names.push_back(std::string(target_block->name));
    mom_dims++;
  }

  std::cout << "Found the following particle data blocks:\n";
  std::cout << id_block->name << "\n";
  std::cout << weight_block->name << "\n";
  for(StringVector::iterator itr = mom_block_names.begin(); itr != mom_block_names.end(); itr++){
    std::cout << *itr << "\n";
  }

  if(mom_dims == 0){
    std::cerr << "Error: No momentum data found.  Cannot continue" << std::endl;
    return(-1);
  } else if(mom_dims < 3){
    std::cerr << "Warning: Only " << mom_dims << " momentum dimensions found. "
                 "This may mean you are missing data.\n" << std::endl;
  }

  sdf_free_blocklist_data(sdf_handle);
  sdf_close(sdf_handle);

  std::cout << "Beginning first pass:\n"
            << "Selecting species " << species << " with gamma > " << gamma_thres << std::endl;

  int pcount = 0;
  int skipcount = 0;

  //iterate over files to preselect particle ids of interest.
  double gamma_thres2 = gamma_thres * gamma_thres;
  for(auto it = sdf_list->begin(); it != sdf_list->end(); it++){
    if(pcount++ % count_freq == 0){
      std::cout << "Progress " << pcount << "/" << sdf_list->size() << "\r" << std::flush;
    }
    sdf_handle = sdf_open(it->filename().string().c_str(), mpi_comm, SDF_READ, mmap);
    sdf_read_blocklist(sdf_handle);

    if((sdf_handle->time < time_min) || (sdf_handle->time > time_max)){
      skipcount++;
      goto LOOPEND;
    }

    if(!sdf_handle){
    std::cerr << "Error: Failed to open file "
              << it->filename().string() << ". Skipping..." << std::endl;
    goto LOOPEND;
    }

    id_block = sdf_find_block_by_name(sdf_handle, ("Particles/ID/" + species).c_str());
    if(!id_block){
    std::cerr << "Error: Particles/ID/" << species << " not present in file "
              << it->filename().string() << ". Skipping..." << std::endl;
    goto LOOPEND;
    }
    sdf_helper_read_data(sdf_handle, id_block);

    weight_block = sdf_find_block_by_name(sdf_handle, ("Particles/Weight/" + species).c_str());
    if(!weight_block){
    std::cerr << "Error: Particles/Weight/" << species << " not present in file "
              << it->filename().string() << ". Skipping..." << std::endl;
    goto LOOPEND;
    }
    sdf_helper_read_data(sdf_handle, weight_block);

    for(int i=0; i<mom_dims; i++){
      mom_blocks[i] = sdf_find_block_by_name(sdf_handle, mom_block_names[i].c_str());
      if(!mom_blocks[i]){
        std::cerr << "Error: " << mom_block_names[i] << " not present in file "
              << it->filename().string() << ". Skipping..." << std::endl;
        goto LOOPEND;
      }

      if( sdf_helper_read_data(sdf_handle, mom_blocks[i]) != 0){
       std::cerr << "Error reading " << mom_block_names[i] << " from file "
        << it->filename().string() << ". Skipping..." << std::endl;
        goto LOOPEND;
      }

    }

    double gamma2;
    for(int64_t i=0; i<id_block->nelements; i++){
      gamma2 = 1;
      for(int j=0; j<mom_dims; j++){
        gamma2 += pow(((double*)(mom_blocks[j]->data))[i]/CONST_mc,2);
      }
      if( gamma2 > gamma_thres2){
        selected_ids.push_back(((int64_t*)(id_block->data))[i]);
      }
    }

    LOOPEND:
    sdf_free_blocklist_data(sdf_handle);
    sdf_close(sdf_handle);
    continue;
  }

  //Deduplicate vector (std::unique requires iterable to be sorted)
  std::sort(selected_ids.begin(),selected_ids.end());
  selected_ids.erase( std::unique(selected_ids.begin(), selected_ids.end()),selected_ids.end());

  std::cout << "\r\nSelection Pass complete. Found " << selected_ids.size()
            << " particles matching criteria.\n" << std::endl;

  if( skipcount > 0 ){
    std::cout << " Skipped " << skipcount << " snapshots outside of time window.\n"
              << std::endl;
  }

  //write list of ids out to a file.
  std::fstream outfile;
  outfile.open("ids.txt", std::fstream::out | std::fstream::trunc);
  for(Int64Vector::iterator itr = selected_ids.begin(); itr != selected_ids.end(); itr++){
    outfile << *itr << std::endl;
  }
  outfile.close();

  if(rand_frac < 1){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    long subset_size = std::lround(selected_ids.size()*rand_frac);
    if(subset_size < 1){ subset_size = 1;}
    std::cout << "Gathering a random fraction (" << rand_frac << ") of particles: "
              << subset_size << "/" << selected_ids.size() << std::endl;
    std::shuffle(selected_ids.begin(), selected_ids.end(), std::default_random_engine(seed));
    selected_ids.resize(subset_size);
  }

  std::cout << "Beginning second pass...\n" << std::endl;

  //Create map of particle vectors
  Int64ParticleVectorMap part_id_map;
  for(Int64Vector::iterator itr = selected_ids.begin(); itr != selected_ids.end(); itr++){
    part_id_map.insert(std::pair<int64_t, ParticleVector>(*itr, ParticleVector()));
    }
  //Now loop to collect particle data
  pcount = 0;
  for(auto it = sdf_list->begin(); it != sdf_list->end(); it++){
    std::cout << ++pcount  << "/" << sdf_list->size() << " "
              << it->filename().string() << std::endl;
    sdf_handle = sdf_open(it->filename().string().c_str(), mpi_comm, SDF_READ, mmap);
    if(!sdf_handle){
    std::cerr << "Error: Failed to open file "
              << it->filename().string() << ". Skipping..." << std::endl;
    goto LOOPEND2;
    }
    sdf_read_blocklist(sdf_handle);

    id_block = sdf_find_block_by_name(sdf_handle, ("Particles/ID/" + species).c_str());
    if(!id_block){
      std::cerr << "Error: Particles/ID/" << species << " not present in file "
                << it->filename().string() << ". Skipping..." << std::endl;
    goto LOOPEND2;
    }
    sdf_helper_read_data(sdf_handle, id_block);

    weight_block = sdf_find_block_by_name(sdf_handle, ("Particles/Weight/" + species).c_str());
    if(!weight_block){
    std::cerr << "Error: Particles/Weight/" << species << " not present in file "
              << it->filename().string() << ". Skipping..." << std::endl;
    goto LOOPEND2;
    }
    sdf_helper_read_data(sdf_handle, weight_block);

    pos_block = sdf_find_block_by_name(sdf_handle, pos_block_name.c_str());
    if(!pos_block){
      std::cerr << "Error: " << pos_block_name << " not present in file "
                << it->filename().string() << ". Skipping..." << std::endl;
      goto LOOPEND2;
    }
    sdf_helper_read_data(sdf_handle, pos_block);

    for(int i=0; i<mom_dims; i++){
      mom_blocks[i] = sdf_find_block_by_name(sdf_handle, mom_block_names[i].c_str());
      if(!mom_blocks[i]){
        std::cerr << "Error: " << mom_block_names[i] << " not present in file "
              << it->filename().string() << ". Skipping..." << std::endl;
        goto LOOPEND2;
      }

      if( sdf_helper_read_data(sdf_handle, mom_blocks[i]) != 0){
       std::cerr << "Error reading " << mom_block_names[i] << " from file "
        << it->filename().string() << ". Skipping..." << std::endl;
        goto LOOPEND2;
      }
    }

    //now loop to collect particle data
    nelem = id_block->nelements;
    for(int64_t i=0; i<nelem; i++){
        int64_t key = ((int64_t*)(id_block->data))[i];
        if(part_id_map.count(key) > 0){
          auto map_ptr = part_id_map.find(key);
          map_ptr->second.push_back(Particle());

          auto& curr_part = map_ptr->second.back();
          curr_part.t = sdf_handle->time*CONST_c/CONST_micro;
          curr_part.q = -((double*)(weight_block->data))[i];

          ins_pos[pos_dims](curr_part, pos_block, i);
          ins_mom[pos_dims](curr_part, mom_blocks, i, nelem);
        }
    }

    LOOPEND2:
    sdf_free_blocklist_data(sdf_handle);
    sdf_close(sdf_handle);
    continue;
  }


  hid_t file_h, group_h, fapl;
  herr_t status, dset_props;

  // set up compact storage layout
  dset_props = H5Pcreate(H5P_DATASET_CREATE);
//  status = H5Pset_layout(dset_props, H5D_COMPACT);

  fapl = H5Pcreate(H5P_FILE_ACCESS);
  status = H5Pset_libver_bounds(fapl, H5F_LIBVER_18, H5F_LIBVER_LATEST);
  file_h = H5Fcreate(hdf5out.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, fapl);

  std::cout << "Writing \"" << hdf5out << "\":" << std::endl;
  pcount =  0;
  size_t pmsize = part_id_map.size();
  hsize_t* dsize;
  double out_step = std::floor(std::log10(pmsize)) - 3;
  if (out_step < 1) { out_step = 1;};
  pcount_freq = std::lround(std::pow(10,out_step));

  ParticleArray* outdata;
  for(auto id_itr = part_id_map.begin(); id_itr != part_id_map.end(); id_itr++){
    if(pcount++ % pcount_freq == 0){
      std::cout << "Progress " << pcount << "/" << pmsize << "\r" << std::flush;
    }
    std::sort(id_itr->second.begin(), id_itr->second.end());
    outdata = particleArrayFromVector(id_itr->second);
    id_itr->second.clear();

    group_h =  H5Gcreate(file_h, std::to_string(id_itr->first).c_str(),
                              H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    dsize = outdata->size;

    status = write_dset(group_h, "t", dsize, outdata->t, dset_props);
    status = write_dset(group_h, "x1", dsize, outdata->x, dset_props);
    status = write_dset(group_h, "x2", dsize, outdata->y, dset_props);
    status = write_dset(group_h, "x3", dsize, outdata->z, dset_props);
    status = write_dset(group_h, "p1", dsize, outdata->px, dset_props);
    status = write_dset(group_h, "p2", dsize, outdata->py, dset_props);
    status = write_dset(group_h, "p3", dsize, outdata->pz, dset_props);
    if (radt_mode){
      status = write_dset(group_h, "ene", dsize, outdata->ene, dset_props);
    } else {
      status = write_dset(group_h, "gamma", dsize, outdata->ene, dset_props);
    }
    status = write_dset(group_h, "q", dsize, outdata->q, dset_props);


    status = H5Gclose(group_h);

  }

  status = H5Fclose(file_h);

  return(0);
}

herr_t write_dset(hid_t group_h, const char* name, hsize_t* dsize, double* data, herr_t dset_props){
    herr_t status;
    hid_t dspace_h, dset_h;

    dspace_h = H5Screate_simple(1,dsize, NULL);
    dset_h = H5Dcreate(group_h, name, H5T_NATIVE_DOUBLE, dspace_h, H5P_DEFAULT, dset_props, H5P_DEFAULT); 
    status = H5Dwrite(dset_h, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    status = H5Dclose(dset_h);
    status = H5Sclose(dspace_h);
    
    return(status);
}
