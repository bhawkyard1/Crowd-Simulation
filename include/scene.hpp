#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <vector>

#include "navPoint.hpp"
#include "actor.hpp"
#include "vectors.hpp"
#include "AABB.hpp"

class scene
{
  std::vector<actor> m_actors;
  std::vector<std::vector<actor *>> m_partitions;
  std::vector<navPoint> navCloud;
public:
  scene();
  void update(float dt);
  void broadPhase(std::vector<actor *> _ents, aabb _box, short int lvl);
  void narrowPhase();
  void draw(float update);
  void addPoint(navPoint _p) {navCloud.push_back(_p);}

  std::vector<actor> * getActors() {return &m_actors;}
};

#endif
