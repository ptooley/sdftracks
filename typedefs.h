#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

#include <map>

#include "particle.h"
#include "boost/filesystem.hpp"

namespace bf = boost::filesystem;

typedef std::vector<bf::path> PathVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<int64_t> Int64Vector;
typedef std::vector<Particle> ParticleVector;
typedef std::map<int64_t, ParticleVector> Int64ParticleVectorMap;
typedef std::map<int64_t, Int64Vector> Int64Int64VectorMap;

#endif
