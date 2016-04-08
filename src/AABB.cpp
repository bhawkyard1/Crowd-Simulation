#include <ciso646>

#include "AABB.hpp"
#include "util.hpp"

vec3 max(const aabb _box)
{
  return _box.m_pos + _box.m_dim;
}

aabb max(const aabb _box1, const aabb _box2)
{
 aabb ret;
 ret.m_pos = {
   fminf(_box1.m_pos.m_x, _box2.m_pos.m_x),
   fminf(_box1.m_pos.m_y, _box2.m_pos.m_y),
   fminf(_box1.m_pos.m_z, _box2.m_pos.m_z)
 };
 ret.m_dim = {
   fmaxf(_box1.m_dim.m_x, _box2.m_dim.m_x),
   fmaxf(_box1.m_dim.m_y, _box2.m_dim.m_y),
   fmaxf(_box1.m_dim.m_z, _box2.m_dim.m_z)
 };
 return ret;
}

bool pointVSAABB(const vec3 _pt, aabb _box)
{
  vec3 m = max(_box);
  return
      inRange(_pt.m_x, _box.m_pos.m_x, m.m_x) and
      inRange(_pt.m_y, _box.m_pos.m_y, m.m_y) and
      inRange(_pt.m_z, _box.m_pos.m_z, m.m_z);
}

std::vector<aabb> divide(const aabb _src)
{
  std::vector<aabb> ret;
  //Ay bby u like nested loops?
  for(int k = 0; k < 2; ++k)
  {
    for(int j = 0; j < 2; ++j)
    {
      for(int i = 0; i < 2; ++i)
      {
        ret.push_back({
                        {_src.m_pos.m_x + (_src.m_dim.m_x / 2.0f) * i,
                         _src.m_pos.m_y + (_src.m_dim.m_y / 2.0f) * j,
                         _src.m_pos.m_z + (_src.m_dim.m_z / 2.0f) * k
                        },
                        _src.m_dim / 2.0f
                      });
      }
    }
  }
  return ret;
}
