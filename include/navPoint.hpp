#ifndef NAVPOINT_HPP
#define NAVPOINT_HPP

#include <vector>
#include "vectors.hpp"

struct navPoint
{
  vec3 m_pos;
  std::vector<navPoint *> m_neighbours;
};

#endif
