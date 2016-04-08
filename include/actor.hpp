#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <cwchar>

#include <vector>
#include "vectors.hpp"

#include "base.hpp"

class actor: public base
{
  float m_speed;
  float m_maxSpeed;
  vec3 m_travelDir;
  vec3 m_tPos;
  std::vector<vec3> m_waypoints;
  size_t m_index;
  float m_stepOffset;
public:
  actor(vec3 _pos);
  void update(float _dt);
  void setTPos(vec3 _p) {m_tPos = _p;}
  void addWaypoint(const vec3 _p) {m_waypoints.push_back(_p);}
  std::vector<vec3> getWaypoints() {return m_waypoints;}
  void updateDir();
  void accelerate(vec3 _dir);
  float offset() {return m_stepOffset;}
  float getMaxSpeed() {return m_maxSpeed;}
};

#endif
