#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "vectors.hpp"

#include "base.hpp"

class actor: public base
{
  float maxSpeed;
public:
  actor(vec3 _pos);
  void update(float dt);
};

#endif
