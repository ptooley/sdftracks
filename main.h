#ifndef _SDFTRACKS_H
#define _SDFTRACKS_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <exception>
#include <algorithm>
#include <chrono>
#include <random>

#include <stdint.h>
#include <cmath>

#include <boost/filesystem.hpp>

#include "hdf5.h"
#include "hdf5_hl.h"

#include "sdf.h"
#include "sdf_helper.h"

#include "configuration.h"

#include "particle.h"
#include "typedefs.h"
#include "constants.h"
#include "filter.h"

namespace bf = boost::filesystem;

herr_t write_dset(hid_t , const char* , hsize_t* , double* , herr_t );

#endif
