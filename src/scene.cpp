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
  //for(auto &i : m_navCloud) ents.push_back(&i);

  //aabb box = enclose(m_navCloud);

  /*std::vector<navPoint *> initEnts;
  for(auto &i : m_navCloud) initEnts.push_back(&i);
  std::sort(initEnts.begin(), initEnts.end(), lessX);
  m_tree.m_node = initEnts[initEnts.size() / 2];

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

  for(auto &i : m_navCloud)
  {
    navPoint * best = nullptr;
    std::vector<std::pair<kdtree*,bool>> path;
    kdtree * input = &m_tree;
    getNearestNavPoint(i.m_pos, input, &best, &path);

    for(auto j = path.end() - 1; j != path.begin() - 1; --j)
    {
      if(magns(j->first->m_node->m_pos - i.m_pos) < magns(best->m_pos - i.m_pos))
      {
         best = j->first->m_node;
      }
    }
    i.m_neighbours.push_back(best);
  }*/

  int count = 0;
  for(auto &i : m_navCloud)
  {
    std::vector<navPoint *> temp;
    std::cout << "Count " << count << std::endl;
    for(auto &j : m_navCloud)
    {
      if(&j == &i) continue;
      if(temp.size() < 4)
      {
        temp.push_back(&j);
        continue;
      }
      else
      {
        temp.erase( temp.begin() + 4, temp.end() );
        float dist = magns(j.m_pos - i.m_pos);
        for(int k = 0; k < 4; ++k)
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

  _cur->m_node = _ents[index];
  _cur->m_axis = -1;

  if(left.size() == 0) _cur->m_children.first = nullptr;
  else _cur->m_children.first = genKDT(newTree, left, _axis, _a);

  if(right.size() == 0) _cur->m_children.second = nullptr;
  else _cur->m_children.second = genKDT(newTree, right, _axis, _a);

  std::cout << "recursion end " << &_cur << std::endl;
  return _cur;
}
/*
std::unique_ptr<kdtree> scene::genKDT(std::unique_ptr<kdtree> _cur, std::vector<navPoint *> _ents, int _axis)
{
    std::cout << "p1" << std::endl;
    if(_axis == 0) std::sort(_ents.begin(), _ents.end(), lessX);
    else if(_axis == 1) std::sort(_ents.begin(), _ents.end(), lessY);
    else if(_axis == 2) std::sort(_ents.begin(), _ents.end(), lessZ);
    std::cout << "p2" << std::endl;
    size_t index = _ents.size() / 2;

    std::vector<navPoint *> left;
    std::vector<navPoint *> right;

    for(size_t i = 0; i < index; ++i) left.push_back(_ents[i]);
    for(size_t i = index + 1; i < _ents.size(); ++i) right.push_back(_ents[i]);
    std::cout << "p3" << std::endl;
    _axis += 1;
    if(_axis > 3) _axis = 0;
    std::cout << "p3.5" << std::endl;
    std::unique_ptr<kdtree> pcur (new kdtree);
    std::cout << "p3.6 " << _ents.size() << ", " << index << std::endl;
    //pcur->m_node = _ents[index];
    //std::cout << "p4" << std::endl;
    if(_ents.size() <= 1)
    {
        std::cout << "nullpt it!" << std::endl;
        pcur->m_node = nullptr;
        pcur->m_children.first = nullptr;
        pcur->m_children.second = nullptr;
    }
    else
    {
        std::cout << "recurse it!" << std::endl;
        pcur->m_node = _ents[index];
        pcur->m_children.first = genKDT(std::move(pcur), left, _axis);
        pcur->m_children.second = genKDT(std::move(pcur), right, _axis);
    }
    std::cout << "recursion end " << &pcur << std::endl;
    return pcur;
}
*/
std::vector<vec3> scene::addActor(navPoint * _p)
{
  std::cout << "yo! " << _p->m_pos.m_x << ", " << _p->m_pos.m_y << ", " << _p->m_pos.m_z << std::endl;
  actor a (_p->m_pos);
  std::cout << "yo! " << a.getPos().m_x << ", " << a.getPos().m_y << ", " << a.getPos().m_z << std::endl;
  navPoint * npt = &m_navCloud[rand() % m_navCloud.size()];
  a.setTPos(npt->getPos());
  m_actors.push_back(a);

  std::vector<vec3> ret = calcPath(&m_actors.back(), _p, npt);
  std::cout << "yo! " << m_actors.back().getPos().m_x << ", " << m_actors.back().getPos().m_y << ", " << m_actors.back().getPos().m_z << std::endl;
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

  //Nodes to remember.
  std::vector<navPoint *> closedList;
  //Add the start node to the closed list.
  closedList.push_back(_start);

  std::cout << "p0" << std::endl;
  bool once = true;
  while((openList.size() > 0 or once) and std::find(closedList.begin(), closedList.end(), _end) == closedList.end())
  {
    std::cout << "p1" << std::endl;
    //Loop through the neighbours of the node at the end of the open list (i.e. node with the lowest f cost.)
    for(auto &i : closedList.back()->m_neighbours)
    {
      std::cout << "p1.1" << std::endl;
      //If the considered node is already on the closed list, skip it.
      if(std::find(closedList.begin(), closedList.end(), i) != closedList.end()) continue;

      bool alreadyOnOpenList = false;
      auto entry = openList.begin();
      std::cout << "p1.2" << std::endl;
      for(auto j = openList.begin(); j != openList.end(); ++j)
      {
        if(j->first.first == i)
        {
          entry = j;
          alreadyOnOpenList = true;
        }
      }
      std::cout << "p1.3" << std::endl;
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
      once = false;
    }
    std::cout << "p2" << std::endl;
    closedList.push_back(openList.back().first.first);
    openList.pop_back();
  }

  for(auto &i : closedList)
  {
    _a->addWaypoint(i->m_pos);
  }

  return _a->getWaypoints();
}

void scene::getNearestNavPoint(vec3 _p, kdtree * _inputNode, navPoint ** _best, std::vector<std::pair<kdtree*, bool>>* _path)
{
  std::cout << "input has null nav? " << (_inputNode->m_node == nullptr) << std::endl;
  //False = left, true = right
  bool dir = false;
  if(_inputNode->m_axis == 0)
  {
    if(_p.m_x > _inputNode->m_node->m_pos.m_x)
    {
      dir = true;
    }
  }
  else if(_inputNode->m_axis == 0)
  {
    if(_p.m_y > _inputNode->m_node->m_pos.m_y)
    {
      dir = true;
    }
  }
  else if(_inputNode->m_axis == 0)
  {
    if(_p.m_z > _inputNode->m_node->m_pos.m_z)
    {
      dir = true;
    }
  }
  /*std::cout << "p1 " << (_inputNode == nullptr) << std::endl;

  std::cout << "p2 " << (_inputNode->m_children.first == nullptr) << ", " << (_inputNode->m_children.second == nullptr) << std::endl;*/

  if(!dir)
  {
    /*std::cout << "go left" << std::endl;
    std::cout << "_p " << _p.m_x << ", " << _p.m_y << ", " << _p.m_z << std::endl;
    std::cout << "_in " << &_inputNode << ", " << (_inputNode == nullptr) << std::endl;
    std::cout << "_best " << _best << ", " << (_best == nullptr) << std::endl;
    std::cout << "_path " << _path << std::endl;*/
    if(_inputNode->m_children.first == nullptr)
    {
      *_best = (_inputNode->m_node);
      std::cout << "bail " << (*_best == nullptr) << ", " << (_inputNode->m_node == nullptr) << std::endl;
      return;
    }
    getNearestNavPoint(_p, _inputNode->m_children.first, _best, _path);
  }
  else
  {
    /*std::cout << "go right" << std::endl;
    std::cout << "_p " << _p.m_x << ", " << _p.m_y << ", " << _p.m_z << std::endl;
    std::cout << "_in " << &_inputNode << ", " << (_inputNode == nullptr) << std::endl;
    std::cout << "_best " << _best << ", " << (_best == nullptr) << std::endl;
    std::cout << "_path " << _path << std::endl;*/
    if(_inputNode->m_children.second == nullptr)
    {
      *_best = (_inputNode->m_node);
      std::cout << "bail " << (*_best == nullptr) << ", " << (_inputNode->m_node == nullptr) << std::endl;
      return;
    }
    getNearestNavPoint(_p, _inputNode->m_children.second, _best, _path);
  }
  _path->push_back({_inputNode, dir});
}
