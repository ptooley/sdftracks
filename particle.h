#ifndef _PARTICLE_H
#define _PARTICLE_H

#include <vector>
#include <stddef.h>
#include <stdint.h>

class Particle{

  public:
    double t =0;
    double x = 0;
    double y = 0;
    double z = 0;
    double px = 0;
    double py = 0;
    double pz = 0;
    double q = 0;
    double ene = 0;


  bool operator<(const Particle& p);

};



class ParticleArray{

  public:
    unsigned long long* size;
    double* t;
    double* x;
    double* y;
    double* z;
    double* px;
    double* py;
    double* pz;
    double* q;
    double* ene;

    ParticleArray(size_t array_length);

    ~ParticleArray();

};


ParticleArray* particleArrayFromVector(std::vector<Particle>& part_array);


#endif
