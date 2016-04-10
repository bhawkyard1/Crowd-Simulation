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

bool operator ==(const navPoint &lhs, const navPoint &rhs);

bool lessX(const navPoint * a, const navPoint * b);
bool lessY(const navPoint * a, const navPoint * b);
bool lessZ(const navPoint * a, const navPoint * b);

#endif
