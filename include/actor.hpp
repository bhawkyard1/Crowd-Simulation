#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <vector>
#include "vectors.hpp"

#include "base.hpp"

class actor: public base
{
  float maxSpeed;
  vec3 tPos;
  std::vector<vec3> m_waypoints;
public:
  actor(vec3 _pos);
  void update(float dt);
  void setTPos(vec3 _p) {tPos = _p;}
};

#endif
