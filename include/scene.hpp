#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <vector>
#include<memory>
#include "navPoint.hpp"
#include "actor.hpp"
#include "vectors.hpp"
#include "AABB.hpp"
#include "kdtree.hpp"

class scene
{
  std::vector<actor> m_actors;
  std::vector<navPoint> m_navCloud;
  std::vector<std::vector<actor*>> m_partitions;
  kdtree m_tree;
public:
  scene();
  void update(float dt);
  void broadPhase(std::vector<actor *> _ents, aabb _box, short int lvl);
  void narrowPhase();
  void draw(float update);
  void addPoint(navPoint _p) {m_navCloud.push_back(_p);}

  void generateNavConnections(const float _threshold);
  void partitionNavs(std::vector< std::vector<navPoint *> > * _partitions, std::vector<navPoint *> _ents, aabb _box, int lvl, int minCount, int maxLvl);

  kdtree * genKDT(kdtree *_cur, std::vector<navPoint *> _ents, int _axis, int * _a);

  std::vector<actor> * getActors() {return &m_actors;}
  std::vector<navPoint> * getNavPoints() {return &m_navCloud;}
  navPoint getNavPoint(size_t i) {return m_navCloud[i];}
  void getNearestNavPoint(vec3 _p, kdtree *_inputNode, navPoint ** _best, std::vector<std::pair<kdtree *, bool> > *_path);

  std::vector<vec3> addActor(navPoint * _p);
  std::vector<vec3> calcPath(actor * _a, navPoint *_start, navPoint *_end);
  void tracePath(actor * _a, navPoint *_start, navPoint *_end);
};

#endif
