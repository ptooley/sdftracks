#ifndef _FILTER_H
#define _FILTER_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

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

void std_1pos(Particle &part, sdf_block_t* block, int64_t i);
void std_2pos(Particle &part, sdf_block_t* block, int64_t i);
void std_3pos(Particle &part, sdf_block_t* block, int64_t i);
void std_1mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len);
void std_2mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len);
void std_3mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len);
void radt_1pos(Particle &part, sdf_block_t* block, int64_t i);
void radt_2pos(Particle &part, sdf_block_t* block, int64_t i);
void radt_3pos(Particle &part, sdf_block_t* block, int64_t i);
void radt_1mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len);
void radt_2mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len);
void radt_3mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len);

#endif
