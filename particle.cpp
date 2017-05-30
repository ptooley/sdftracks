#include "particle.h"

bool Particle::operator<(const Particle& p){
    return(this->t < p.t);
  }


ParticleArray::ParticleArray(size_t array_length){
  //allocate memory
  this->size = new unsigned long long;
  *(this->size) = array_length;
  this->t = new double[array_length];
  this->x = new double[array_length];
  this->y = new double[array_length];
  this->z = new double[array_length];
  this->px = new double[array_length];
  this->py = new double[array_length];
  this->pz = new double[array_length];
  this->q = new double[array_length];
  this->ene = new double[array_length];
}

ParticleArray::~ParticleArray(){
  //deallocate memory
  delete[] this->t;
  delete[] this->x;
  delete[] this->y;
  delete[] this->z;
  delete[] this->px;
  delete[] this->py;
  delete[] this->pz;
}



ParticleArray* particleArrayFromVector(std::vector<Particle>& part_vector){
  
  ParticleArray* part_array;
  size_t array_length = part_vector.size();
  auto part_itr = part_vector.begin();

  part_array = new ParticleArray(array_length);

  
  for(size_t i = 0; i < array_length; i++){
    part_array->t[i] = part_itr->t;
    part_array->x[i] = part_itr->x;
    part_array->y[i] = part_itr->y;
    part_array->z[i] = part_itr->z;
    part_array->px[i] = part_itr->px;
    part_array->py[i] = part_itr->py;
    part_array->pz[i] = part_itr->pz;
    part_array->q[i] = part_itr->q;
    part_array->ene[i] = part_itr->ene;
    part_itr++;
  }

  return(part_array);

}
