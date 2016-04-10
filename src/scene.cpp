#include <ciso646>
#include <algorithm>
#include <array>

#include "scene.hpp"
#include "util.hpp"

scene::scene()
{

}

void scene::update(float _dt)
{
  for(auto &i : m_actors)
  {
    i.update(_dt);
  }

  std::vector<actor *> ents;
  for(auto &i : m_actors) ents.push_back(&i);
  aabb box = enclose(ents);
  broadPhase(ents, box, 0);
}

void scene::draw(float dt)
{

}

void scene::broadPhase(std::vector<actor *> _ents, aabb _box, short lvl)
{
  int count = 0;
  std::vector<actor *> pass;

  for(auto &i : _ents)
  {
    if(pointVSAABB(i->getPos(), _box))
    {
      count++;
      pass.push_back(i);
    }
  }

  if(lvl > 10 or count < 8)
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
      broadPhase(_ents, i, ++lvl);
    }
  }
}

void scene::generateNavConnections(const float _threshold)
{
  aabb box = enclose(m_navCloud);

  std::vector<navPoint *> initEnts;
  for(auto &i : m_navCloud) initEnts.push_back(&i);
  std::sort(initEnts.begin(), initEnts.end(), lessX);
  m_tree.m_node = initEnts[initEnts.size() / 2];
  m_tree.m_parent = nullptr;

  std::vector<navPoint *> left;
  std::vector<navPoint *> right;

  for(size_t i = 0; i < initEnts.size() / 2; ++i) left.push_back(initEnts[i]);
  for(size_t i = initEnts.size() / 2 + 1; i < initEnts.size(); ++i) right.push_back(initEnts[i]);

  int a = 0;
  std::cout << "pre" << std::endl;
  kdtree * in = &m_tree;
  std::cout << "pre2" << std::endl;
  m_tree.m_children.first = genKDT(in, left, 0, &a);
  m_tree.m_children.second = genKDT(in, right, 0, &a);


  std::cout << "post " << a << std::endl;

  /*for(auto &i : m_navCloud)
  {
    std::vector<kdtree*> path;
    kdtree * input = &m_tree;
    kNearestNeighbours(input, input->m_node->m_pos, &path, 4);

    for(auto &j : path) i.m_neighbours.push_back(j->m_node);
  }*/

  int count = 0;
  for(auto &i : m_navCloud)
  {
    std::vector<navPoint *> temp;
    std::cout << "Count " << count << std::endl;
    for(auto &j : m_navCloud)
    {
      if(&j == &i) continue;
      if(temp.size() < 8)
      {
        temp.push_back(&j);
        continue;
      }
      else
      {
        temp.erase( temp.begin() + 8, temp.end() );
        float dist = magns(j.m_pos - i.m_pos);
        for(int k = 0; k < 8; ++k)
        {
          if(magns(temp[k]->m_pos - i.m_pos) > dist)
          {
            temp.insert(temp.begin() + k, &j);
            break;
          }
        }
      }
    }
    i.m_neighbours = temp;
    count++;
  }
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

kdtree * scene::genKDT(kdtree * _cur, std::vector<navPoint *> _ents, int _axis, int * _a)
{
  std::cout << "a is " << *_a << std::endl;
  (*_a)++;

  if(_axis == 0) std::sort(_ents.begin(), _ents.end(), lessX);
  else if(_axis == 1) std::sort(_ents.begin(), _ents.end(), lessY);
  else if(_axis == 2) std::sort(_ents.begin(), _ents.end(), lessZ);
  size_t index = _ents.size() / 2;

  //std::cout << "axis : " << _axis << std::endl;
  //for(auto &i : _ents) std::cout << i->m_pos.m_x << ", " << i->m_pos.m_y << ", " << i->m_pos.m_z << std::endl;

  std::vector<navPoint *> left;
  std::vector<navPoint *> right;

  for(size_t i = 0; i < index; ++i) left.push_back(_ents[i]);
  for(size_t i = index + 1; i < _ents.size(); ++i) right.push_back(_ents[i]);

  _axis += 1;
  if(_axis > 3) _axis = 0;

  kdtree * newTree = new kdtree;
  newTree->m_parent = _cur;

  _cur->m_node = _ents[index];
  _cur->m_axis = -1;

  if(left.size() == 0) _cur->m_children.first = nullptr;
  else _cur->m_children.first = genKDT(newTree, left, _axis, _a);

  if(right.size() == 0) _cur->m_children.second = nullptr;
  else _cur->m_children.second = genKDT(newTree, right, _axis, _a);

  std::cout << "recursion end " << &_cur << std::endl;
  return _cur;
}


std::vector<vec3> scene::addActor(navPoint * _p)
{
  //std::cout << "yo! " << _p->m_pos.m_x << ", " << _p->m_pos.m_y << ", " << _p->m_pos.m_z << std::endl;
  actor a (_p->m_pos);
  //std::cout << "yo! " << a.getPos().m_x << ", " << a.getPos().m_y << ", " << a.getPos().m_z << std::endl;
  navPoint * npt = &m_navCloud[rand() % m_navCloud.size()];
  a.setTPos(npt->getPos());
  m_actors.push_back(a);

  std::vector<vec3> ret = calcPath(&m_actors.back(), _p, npt);
  //std::cout << "yo! " << m_actors.back().getPos().m_x << ", " << m_actors.back().getPos().m_y << ", " << m_actors.back().getPos().m_z << std::endl;
  return ret;
}

std::vector<vec3> scene::calcPath(actor *_a, navPoint *_start, navPoint *_end)
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

  //Nodes to remember. Order: node, parent
  std::vector<std::pair<navPoint *, navPoint *>> closedList;
  //Add the start node to the closed list.
  closedList.push_back({_start, nullptr});

  bool endFound = false;
  //std::cout << "p0" << std::endl;
  bool once = true;
  while((openList.size() > 0 or once) and !endFound)
  {
    //Loop through the neighbours of the node at the end of the open list (i.e. node with the lowest f cost.)
    for(auto &i : closedList.back().first->m_neighbours)
    {
      bool alreadyOnClosedList = false;
      for(auto &q : closedList)
      {
        if(q.first == i) alreadyOnClosedList = true;
      }
      if(alreadyOnClosedList) continue;

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
        vec3 nodePos = i->getPos();
        vec3 goalPos = _end->getPos();
        vec3 diff = nodePos - goalPos;

        float cost = mag(i->getPos() - closedList.back().first->getPos()) * (1 / i->m_weight);
        float dist = mag(i->getPos() - _end->getPos());
        //float dist = 1.0f * (fabs(diff.m_x) + fabs(diff.m_y + fabs(diff.m_z))) + (1.4f - 2.0f) * fmin(diff.m_x, diff.m_y);

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
            openList.insert(k, {{i,{cost,dist}},closedList.back().first});
            addAtEnd = false;
            break;
          }
        }
        //If the f cost has NOT been beaten, just stick it on the end of the open list.
        if(addAtEnd) openList.push_back({{i,{cost,dist}},closedList.back().first});
      }
      else
      {
        //If the neighbour is on the open list, evaluate its dist vs the current square.
        float dist = mag(closedList.back().first->getPos() - _end->getPos());
        if(dist > entry->first.second[1])
        {
          entry->second = closedList.back().first;
        }
      }
      once = false;
    }
    //std::cout << "p2" << std::endl;
    closedList.push_back({openList.back().first.first, openList.back().second});
    if(closedList.back().first == _end) endFound = true;
    openList.pop_back();
  }

  //Step backwards through the closed list, from child to parent, and add to the final vector.
  std::vector<navPoint*> reversed;
  reversed.push_back(closedList.back().first);
  auto cur = closedList.back();
  while(cur.first != _start)
  {
    //std::cout << "pre-for" << std::endl;
    navPoint * toSearch = cur.second;
    for(size_t i = 0; i < closedList.size(); ++i)
    {
      //std::cout << "check" << std::endl;
      if(closedList[i].first == toSearch)
      {
        //std::cout << "MATCH!" << std::endl;
        cur = closedList[i];
        reversed.push_back(cur.first);
        break;
      }
    }
  }

  reverse(reversed.begin(), reversed.end());

  /*for(auto &i : closedList)
  {
    _a->addWaypoint(i.first->m_pos);
  }*/

  for(auto &i : reversed)
  {
    _a->addWaypoint(i->m_pos);
  }

  return _a->getWaypoints();
}
