#ifndef AABB
#define AABB

#include <vector>
#include "vectors.hpp"

struct aabb
{
  vec3 m_pos;
  vec3 m_dim;
};

vec3 max(const aabb _box);
bool pointVSAABB(const vec3 _pt, const aabb _box);
std::vector<aabb> divide(const aabb _src);

#endif
