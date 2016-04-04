#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <vector>

#include "navPoint.hpp"
#include "actor.hpp"
#include "vectors.hpp"
#include "AABB.hpp"

struct partition
{
  std::vector<actor *> m_actors;
  std::vector<navPoint *> m_navs;
};

class scene
{
  std::vector<actor> m_actors;
  std::vector<navPoint> m_navCloud;
  std::vector<partition> m_partitions;
public:
  scene();
  void update(float dt);
  void broadPhase(std::vector<actor *> _ents, std::vector<navPoint *> _p, aabb _box, short int lvl);
  void narrowPhase();
  void draw(float update);
  void addPoint(navPoint _p) {m_navCloud.push_back(_p);}

  void generateNavConnections(const float _threshold);
  void partitionNavs(std::vector< std::vector<navPoint *> > * _partitions, std::vector<navPoint *> _ents, aabb _box, int lvl, int minCount, int maxLvl);

  std::vector<actor> * getActors() {return &m_actors;}
  std::vector<navPoint> * getNavPoints() {return &m_navCloud;}

  void addActor(const vec3 _p);
  void tracePath(actor * _a, navPoint *_start, navPoint *_end);
};

#endif
