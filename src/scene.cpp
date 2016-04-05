#include <ciso646>
#include <algorithm>
#include <array>

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

  std::vector<navPoint *> initEnts;
  for(auto &i : m_navCloud) initEnts.push_back(&i);
  std::sort(initEnts.begin(), initEnts.end(), lessX);
  tree.m_node = initEnts[initEnts.size() / 2];

  std::vector<navPoint *> left;
  std::vector<navPoint *> right;

  for(size_t i = 0; i < initEnts.size() / 2; ++i) left.push_back(initEnts[i]);
  for(size_t i = initEnts.size() / 2 + 1; i < initEnts.size(); ++i) right.push_back(initEnts[i]);

  std::unique_ptr<kdtree> in (&tree);
  tree.m_children.first = genKDT(std::move(in), left, 1);
  tree.m_children.second = genKDT(std::move(in), right, 1);


  //Partition navPoints
  /*partitionNavs(&buckets, ents, box, 0, 32, 8);

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
  }*/
}

void scene::partitionNavs(std::vector< std::vector<navPoint *> > * _partitions, std::vector<navPoint *> _ents, aabb _box, int lvl, int minCount, int maxLvl)
{
  int count = 0;
  std::vector<navPoint *> pass;

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

std::unique_ptr<kdtree> scene::genKDT(std::unique_ptr<kdtree> _cur, std::vector<navPoint *> _ents, int _axis)
{
  if(_axis == 0) std::sort(_ents.begin(), _ents.end(), lessX);
  else if(_axis == 1) std::sort(_ents.begin(), _ents.end(), lessY);
  else if(_axis == 2) std::sort(_ents.begin(), _ents.end(), lessZ);

  size_t index = _ents.size() / 2;

  std::vector<navPoint *> left;
  std::vector<navPoint *> right;

  for(size_t i = 0; i < index; ++i) left.push_back(_ents[i]);
  for(size_t i = index + 1; i < _ents.size(); ++i) right.push_back(_ents[i]);

  _axis += 1;
  if(_axis > 3) _axis = 0;

  std::unique_ptr<kdtree> pcur;
  pcur->m_node = _ents[index];

  if(_ents.size() <= 1)
  {
    pcur->m_children.first = nullptr;
    pcur->m_children.second = nullptr;
  }
  else
  {
    pcur->m_children.first = genKDT(std::move(pcur), left, _axis);
    pcur->m_children.second = genKDT(std::move(pcur), right, _axis);
  }
  return pcur;
}

void scene::addActor(const vec3 _p)
{
  actor a (_p);
  navPoint * npt = &m_navCloud[rand() % m_navCloud.size()];
  a.setTPos(npt->getPos());
  m_actors.push_back(a);
}

void scene::calcPath(actor *_a, navPoint *_start, navPoint *_end)
{
  //Nodes to consider.
  //Stored as follows:
  //  {node, {cost, dist}},
  //  parent
  //ORDERED BY F COST
  std::vector<
      std::pair<
      std::pair<navPoint*, std::array<float, 2>>,
      navPoint*>
      > openList;

  //Nodes to remember.
  std::vector<navPoint *> closedList;

  //Add the start node to the closed list.
  closedList.push_back(_start);

  while(openList.size() > 0 and std::find(closedList.begin(), closedList.end(), _end) == closedList.end())
  {
    //Loop through the neighbours of the node at the end of the open list (i.e. node with the lowest f cost.)
    for(auto &i : closedList.back()->m_neighbours)
    {
      //If the considered node is already on the closed list, skip it.
      if(std::find(closedList.begin(), closedList.end(), i) != closedList.end()) continue;

      bool alreadyOnOpenList = false;
      auto entry = openList.begin();
      for(auto j = openList.begin(); j != openList.end(); ++j)
      {
        if(j->first.first == i)
        {
          entry = j;
          alreadyOnOpenList = true;
        }
      }

      if(!alreadyOnOpenList)
      {
        //If the neigbour is NOT on the open list, insert in the correct place.
        float cost = mag(i->getPos() - closedList.back()->getPos());
        float dist = mag(i->getPos() - _end->getPos());

        bool addAtEnd = true;
        //For each node in the open list, if it beats the current f cost, insert current node just before, then break.
        for(auto k = openList.begin(); k != openList.end(); ++k)
        {
          if(k->first.second[0] + k->first.second[1] < cost + dist)
          {
            //Insert at k a pair, containing:
            //  Another pair, containing:
            //    The current neighbour, and an array, containing:
            //      The cost, and the dist.
            //  And the parent.
            openList.insert(k, {{i,{cost,dist}},closedList.back()});
            addAtEnd = false;
            break;
          }
        }
        //If the f cost has NOT been beaten, just stick it on the end of the open list.
        if(addAtEnd) openList.push_back({{i,{cost,dist}},closedList.back()});
      }
      else
      {
        //If the neighbour is on the open list, evaluate its dist vs the current square.
        float dist = mag(closedList.back()->getPos() - _end->getPos());
        if(dist > entry->first.second[1])
        {
          entry->second = closedList.back();
        }
      }
    }
    closedList.push_back(openList.back().first.first);
    openList.pop_back();
  }
}
