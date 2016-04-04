#ifndef AABB
#define AABB

#include <iostream>
#include <limits>
#include <vector>
#include "vectors.hpp"

#define F_MAX std::numeric_limits<float>::max()
#define F_INF std::numeric_limits<float>::infinity()

#define I_MAX std::numeric_limits<int>::max()
#define I_INF std::numeric_limits<int>::max()

struct aabb
{
  vec3 m_pos;
  vec3 m_dim;
};

vec3 max(const aabb _box);
bool pointVSAABB(const vec3 _pt, const aabb _box);
std::vector<aabb> divide(const aabb _src);

template<typename tt>
aabb enclose(std::vector<tt> _objs)
{
  vec3 min = {F_INF, F_INF, F_INF};
  vec3 max = {-F_INF, -F_INF, -F_INF};

  for(auto &i : _objs)
  {
    vec3 p = i.getPos();

    if(p.m_x < min.m_x) min.m_x = p.m_x;
    if(p.m_y < min.m_y) min.m_y = p.m_y;
    if(p.m_z < min.m_z) min.m_z = p.m_z;

    if(p.m_x > max.m_x) max.m_x = p.m_x;
    if(p.m_y > max.m_y) max.m_y = p.m_y;
    if(p.m_z > max.m_z) max.m_z = p.m_z;
  }

  aabb ret = {min, max - min};

  std::cout << "LEN " << _objs.size() << std::endl;
  std::cout << "GENERATED AABB: (" << ret.m_pos.m_x << ", " << ret.m_pos.m_y << ", " << ret.m_pos.m_z << ") ("
            << ret.m_dim.m_x << ", " << ret.m_dim.m_y << ", " << ret.m_dim.m_z << ")" << std::endl;

  return ret;
}

#endif
