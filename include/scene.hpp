#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <vector>

#include "actor.hpp"
#include "vectors.hpp"
#include "AABB.hpp"

class scene
{
  std::vector<actor> m_actors;
  std::vector<std::vector<actor *>> m_partitions;
public:
  scene();
  void update(float dt);
  void broadPhase(std::vector<actor *> _ents, aabb _box, short int lvl);
  void narrowPhase();
  void draw(float update);
};

#endif
