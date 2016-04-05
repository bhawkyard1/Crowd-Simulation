#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <vector>
#include<memory>
#include "navPoint.hpp"
#include "actor.hpp"
#include "vectors.hpp"
#include "AABB.hpp"

struct partition
{
  std::vector<actor *> m_actors;
  std::vector<navPoint *> m_navs;
};

struct kdtree
{
  navPoint * m_node;
  std::pair<std::unique_ptr<kdtree>, std::unique_ptr<kdtree>> m_children;
};

class scene
{
  std::vector<actor> m_actors;
  std::vector<navPoint> m_navCloud;
  std::vector<partition> m_partitions;
  kdtree tree;
public:
  scene();
  void update(float dt);
  void broadPhase(std::vector<actor *> _ents, std::vector<navPoint *> _p, aabb _box, short int lvl);
  void narrowPhase();
  void draw(float update);
  void addPoint(navPoint _p) {m_navCloud.push_back(_p);}

  void generateNavConnections(const float _threshold);
  void partitionNavs(std::vector< std::vector<navPoint *> > * _partitions, std::vector<navPoint *> _ents, aabb _box, int lvl, int minCount, int maxLvl);

  std::unique_ptr<kdtree> genKDT(std::unique_ptr<kdtree> _cur, std::vector<navPoint *> _ents, int _axis);

  std::vector<actor> * getActors() {return &m_actors;}
  std::vector<navPoint> * getNavPoints() {return &m_navCloud;}
  navPoint getNavPoint(size_t i) {return m_navCloud[i];}

  void addActor(const vec3 _p);
  void calcPath(actor * _a, navPoint *_start, navPoint *_end);
  void tracePath(actor * _a, navPoint *_start, navPoint *_end);
};

#endif
