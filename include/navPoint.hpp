#ifndef NAVPOINT_HPP
#define NAVPOINT_HPP

#include <vector>
#include "vectors.hpp"

struct navPoint
{
  vec3 m_pos;
  std::vector<navPoint *> m_neighbours;
  float m_weight;

  vec3 getPos() {return m_pos;}
};

#endif
