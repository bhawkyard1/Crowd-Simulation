#include "vectors.hpp"
#include "navPoint.hpp"

struct kdtree
{
  int m_axis;
  navPoint * m_node;
  kdtree * m_parent;
  std::pair<kdtree*, kdtree*> m_children;
};

void nearestNeighbour(kdtree * _cur, vec3 _p, kdtree * _best);
void kNearestNeighbours(kdtree * _cur, vec3 _p, std::vector<kdtree *> * _kbest, int _len);
