#include "filter.h"

void std_1pos(Particle &part, sdf_block_t* block, int64_t i){
  part.x = ((double**)(block->grids))[0][i];
}

void std_2pos(Particle &part, sdf_block_t* block, int64_t i){
  part.x = ((double**)(block->grids))[0][i];
  part.y = ((double**)(block->grids))[1][i];
}

void std_3pos(Particle &part, sdf_block_t* block, int64_t i){
  part.x = ((double**)(block->grids))[0][i];
  part.y = ((double**)(block->grids))[1][i];
  part.z = ((double**)(block->grids))[2][i];
}

void std_1mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len){
  part.px = ((double*)(block[0]->data))[i];
  part.ene = sqrt( 1 + pow(part.px/CONST_mc,2));
}

void std_2mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len){
  part.px = ((double*)(block[0]->data))[i];
  part.py = ((double*)(block[1]->data))[i];
  part.ene = sqrt( 1 + pow(part.px/CONST_mc,2) 
                     + pow(part.py/CONST_mc,2));
}

void std_3mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len){
  part.px = ((double*)(block[0]->data))[i];
  part.py = ((double*)(block[1]->data))[i];
  part.pz = ((double*)(block[2]->data))[i];
  part.ene = sqrt( 1 + pow(part.px/CONST_mc,2) 
                     + pow(part.py/CONST_mc,2) 
                     + pow(part.pz/CONST_mc,2));
}

void radt_1pos(Particle &part, sdf_block_t* block, int64_t i){
  part.x = ((double**)(block->grids))[0][i]/CONST_micro;
}

void radt_2pos(Particle &part, sdf_block_t* block, int64_t i){
  part.x = ((double**)(block->grids))[0][i]/CONST_micro;
  part.y = ((double**)(block->grids))[1][i]/CONST_micro;
}

void radt_3pos(Particle &part, sdf_block_t* block, int64_t i){
  part.x = ((double**)(block->grids))[0][i]/CONST_micro;
  part.y = ((double**)(block->grids))[1][i]/CONST_micro;
  part.z = ((double**)(block->grids))[2][i]/CONST_micro;
}

void radt_1mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len){
  part.px = ((double*)(block[0]->data))[i]/CONST_mc;
  part.ene = sqrt( 1 + pow(part.px,2)) - 1;
}

void radt_2mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len){
  part.px = ((double*)(block[0]->data))[i]/CONST_mc;
  part.py = ((double*)(block[1]->data))[i]/CONST_mc;
  part.ene = sqrt( 1 + pow(part.px,2) + pow(part.py,2)) - 1;
}

void radt_3mom(Particle &part, sdf_block_t** block, int64_t i, int64_t len){
  part.px = ((double*)(block[0]->data))[i]/CONST_mc;
  part.py = ((double*)(block[1]->data))[i]/CONST_mc;
  part.pz = ((double*)(block[2]->data))[i]/CONST_mc;
  part.ene = sqrt( 1 + pow(part.px,2) + pow(part.py,2) + pow(part.pz,2)) - 1;
}
