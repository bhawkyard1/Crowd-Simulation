#include <ciso646>
#include <algorithm>

#include "scene.hpp"
#include "util.hpp"

scene::scene()
{

}

void scene::update(float dt)
{

}

void scene::draw(float dt)
{

}

void scene::broadPhase(std::vector<actor *> _ents, std::vector<navPoint *> _p, aabb _box, short lvl)
{
  int count = 0;
  partition pass;

  for(auto &i : _ents)
  {
    if(pointVSAABB(i->getPos(), _box))
    {
      count++;
      pass.m_actors.push_back(i);
    }
  }
  for(auto &i : _p)
  {
    if(pointVSAABB(i->getPos(), _box))
    {
      count++;
      pass.m_navs.push_back(i);
    }
  }


  if(lvl > 10 or count < 32)
  {
    //Return
    m_partitions.push_back(pass);
  }
  else
  {
    //Divide
    std::vector<aabb> boxes = divide(_box);
    for(auto &i : boxes)
    {
      broadPhase(_ents, _p, i, ++lvl);
    }
  }
}

void scene::generateNavConnections(const float _threshold)
{
  std::vector< std::vector< navPoint * > > buckets;
  std::vector< navPoint * > ents;

  for(auto &i : m_navCloud) ents.push_back(&i);

  aabb box = enclose(m_navCloud);

  //Partition navPoints
  partitionNavs(&buckets, ents, box, 0, 32, 8);

  int DELETE_count = 0;

  //Sort navPoints
  for(auto &bucket : buckets)
  {
    for(auto &navA : bucket)
    {
      //This will be a vector of connections to navA.
      std::vector<navPoint *> connections;
      for(auto &navB : bucket)
      {
        //Don't make navA a connection to navA...
        if(navA == navB) continue;
        //Calc distance
        float dist = magns(navB->getPos() - navA->getPos());
        //If there are under 4 connections, add this.
        if(connections.size() == 0 and dist < _threshold * _threshold)
        {
          connections.push_back(navB);
        }
        else
        {
          //Iterate over connections, insert in place.
          for(int i = 0; i < connections.size(); ++i)
          {
            float cdist = magns(connections.at(i)->getPos() - navA->getPos());
            if(dist < cdist and dist < _threshold * _threshold)
            {
              connections.insert(connections.begin() + i, navB);
              break;
            }
          }
        }
        //If there are too many connections, cull the furthest.
        if(connections.size() > 4) connections.erase(connections.begin() + 4, connections.end());
      }
      navA->m_neighbours = connections;
    }
  }
}

void scene::partitionNavs(std::vector< std::vector<navPoint *> > * _partitions, std::vector<navPoint *> _ents, aabb _box, int lvl, int minCount, int maxLvl)
{
  std::cout << "CALL " << lvl << std::endl;
  int count = 0;
  //std::cout << "  p0" << std::endl;
  std::vector<navPoint *> pass;
  //std::cout << "  p1" << std::endl;

  for(auto &i : _ents)
  {
    if(pointVSAABB(i->getPos(), _box))
    {
      count++;
      pass.push_back(i);
    }
  }

  if(lvl > maxLvl or count < minCount)
  {
    //Return
    _partitions->push_back(pass);
  }
  else
  {
    //Divide
    std::vector<aabb> boxes = divide(_box);
    for(auto &i : boxes)
    {
      partitionNavs(_partitions, _ents, i, ++lvl, minCount, maxLvl);
    }
  }
}

void scene::addActor(const vec3 _p)
{
  actor a (_p);
  navPoint * npt = &m_navCloud[rand() % m_navCloud.size()];
  a.setTPos(npt->getPos());
  m_actors.push_back(a);
}

void scene::tracePath(actor * _a, navPoint * _start, navPoint * _end)
{
  navPoint * cur = _start;

  //These are nodes that have been considered but not used. They will not be used in future calculations.
  std::vector<navPoint *> open;
  //These are nodes that the actor will use.
  std::vector<navPoint *> use;

  //Trace start to end.
  while(cur != _end)
  {
    float bestHeuristic = F_INF;
    navPoint * bestPoint;

    //Find the best point.
    for(auto &point : cur->m_neighbours)
    {
      //Check the considered point has not already been evaluated.
      if(std::find(open.begin(), open.end(), point) != open.end())
      {
        continue;
      }

      float cost = magns(point->getPos() - cur->getPos()) * point->m_weight;
      float dist = magns(point->getPos() - _end->getPos());
      float heuristic = cost + dist;
      if(heuristic < bestHeuristic)
      {
        bestHeuristic = heuristic;
        bestPoint = point;
      }
    }

    //Put the best point in the closed list, the rest in the open list.
    for(auto &point : cur->m_neighbours)
    {
      if(point == bestPoint)
      {
        use.push_back(point);
        cur = use.back();
      }
      else open.push_back(point);
    }
  }
}
