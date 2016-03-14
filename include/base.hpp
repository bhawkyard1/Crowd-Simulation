#ifndef BASE_HPP
#define BASE_HPP

#include "vectors.hpp"

class base
{
  vec3 m_pos, m_ppos, m_vel;
public:
  void setPos(vec3 _pos) {m_pos = _pos;}
  vec3 getPos() {return m_pos;}

  vec2 getPPos() {return prevPos;}
  void setPPos(vec2 p) {prevPos = p;}

  void setVel(vec3 _vel) {m_vel = _vel;}
  void addVel(vec3 _vel) {m_vel += _vel;}
  vec3 getVel() {return m_vel;}

  void updatePos(float dt) { m_ppos = m_pos; m_pos += dt * m_vel;}
};

#endif
