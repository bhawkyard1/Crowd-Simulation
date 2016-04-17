#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <cwchar>
#include <array>
#include <vector>
#include "vectors.hpp"

#include "base.hpp"

class actor: public base
{
  float m_speed;
  float m_maxSpeed;
  vec3 m_travelDir;
  vec3 m_tPos;
  bool m_recalcPhys;
  std::vector< std::pair<vec3, vec3> > m_waypoints;
  std::pair<vec3, vec3>  m_collisionData;
  size_t m_index;
  float m_stepOffset;
  std::array<float, 4> m_col;
public:
  actor(vec3 _pos);
  void update(float _dt);
  void setTPos(vec3 _p) {m_tPos = _p;}
  void addWaypoint(const vec3 _p, const vec3 _n) {m_waypoints.push_back({_p, _n});}
  std::vector< std::pair<vec3, vec3> > getWaypoints() {return m_waypoints;}
  void updateDir();
  void accelerate(vec3 _dir);
  float offset() {return m_stepOffset;}
  float getMaxSpeed() {return m_maxSpeed;}
  std::array<float, 4> getCol() {return m_col;}
  void physUpdate();
  void setCollisionData(std::pair<vec3, vec3> _data) {m_collisionData = _data; m_recalcPhys = false;}
  std::pair<vec3, vec3> getCollisionData() {return m_collisionData;}
  bool recalcCol() {return m_recalcPhys;}
};

#endif
