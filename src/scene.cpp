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
  narrowPhase();
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

void scene::narrowPhase()
{
  for(auto &i : m_partitions)
  {
    if(i.size() == 0) continue;

    vec3 averagePosition = {0.0f, 0.0f, 0.0f};
    for(auto &actor : i)
    {
      averagePosition += actor->getPos();
    }
    averagePosition = averagePosition / static_cast<float>(i.size());

    for(auto &actor : i)
    {
      vec3 diff = actor->getPos() - averagePosition;
      float dist = mag(diff);
      if(dist > 0.0f) diff = diff / sqr(dist);
      else diff = {0.0f, 0.0f, 0.0f};
      actor->accelerate(diff);
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
    int con = 8;
    for(auto &j : m_navCloud)
    {
      if(&j == &i) continue;
      if(temp.size() < con)
      {
        temp.push_back(&j);
        continue;
      }
      else
      {
        temp.erase( temp.begin() + con, temp.end() );
        float dist = magns(j.m_pos - i.m_pos);
        for(int k = 0; k < con; ++k)
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
  std::cout << "p1" << std::endl;
  //Nodes to consider.
  //Stored as follows:
  //  {node, {cost, dist}},
  //  parent
  //ORDERED BY F COST
  std::vector<astarNode> openList;

  //Nodes to remember. Order: node, parent
  std::vector<astarNode> closedList;

  //Add the start node to the closed list.
  //closedList.push_back({_start, 0.0f, mag(_start->getPos() - _end->getPos()), nullptr});
  openList.push_back({_start, 0.0f, mag(_start->getPos() - _end->getPos()), nullptr});

  bool endFound = false;

  while(openList.size() > 0 and !endFound)
  {
    astarNode expandingNode = openList.back();

    //Loop through the neighbours of the node at the end of the open list (i.e. node with the lowest f cost.)
    for(auto &i : expandingNode.m_node->m_neighbours)
    {
      //Check if the neighbour is already on the closed list.
      bool alreadyOnClosedList = false;
      for(auto &q : closedList)
      {
        if(q.m_node == i) alreadyOnClosedList = true;
      }
      //Skip if it on the closed list.
      if(alreadyOnClosedList) continue;

      //Check if the neighbour is already on the open list.
      bool alreadyOnOpenList = false;
      auto entry = openList.begin();

      for(auto j = openList.begin(); j != openList.end(); ++j)
      {
        if(j->m_node == i)
        {
          entry = j;
          alreadyOnOpenList = true;
        }
      }
      //If the node is not already on the open list, insert it in the proper place.
      if(!alreadyOnOpenList)
      {
        //If the neigbour is NOT on the open list, insert in the correct place.
        vec3 nodePos = i->getPos();
        vec3 goalPos = _end->getPos();
        vec3 diff = nodePos - goalPos;

        float cost = expandingNode.m_cost + mag(i->getPos() - expandingNode.m_node->getPos()) /* (1 / i->m_weight)*/;
        float dist = mag(i->getPos() - _end->getPos());

        bool addAtEnd = true;
        //For each node in the open list, if it beats the current f cost, insert current node just before, then break.
        for(auto k = openList.begin(); k != openList.end(); ++k)
        {
          if(k->m_cost + k->m_dist < cost + dist)
          {
            //Insert at k an a-star node, containing:
            openList.insert(k, {i, cost, dist, expandingNode.m_node});
            addAtEnd = false;
            break;
          }
        }
        //If the f cost has NOT been beaten, just stick it on the end of the open list.
        if(addAtEnd) openList.push_back({i, cost, dist, expandingNode.m_node});
      }
      //If the node IS already on the open list and it is better than the current node, calculate costs, and move it to the right place.
      else
      {
        float cost = expandingNode.m_cost + mag(entry->m_node->getPos() - expandingNode.m_node->getPos()) /* (1 / i->m_weight)*/;
        if(cost < entry->m_cost)
        {
          entry->m_parent = expandingNode.m_node;

          //Move entry to the correct place.
          astarNode temp = *entry;
          openList.erase(entry);

          for(auto j = openList.begin(); j != openList.end(); ++j)
          {
            if(j->m_cost + j->m_dist < temp.m_cost + temp.m_dist)
            {
              openList.insert(j, temp);
              break;
            }
          }
        }
      }
    }

    closedList.push_back(expandingNode);
    if(closedList.back().m_node == _end) endFound = true;

    for(auto i = openList.begin(); i != openList.end(); ++i)
    {
      if(i->m_parent == expandingNode.m_parent and i->m_node == expandingNode.m_node)
      {
        openList.erase(i);
        break;
      }
    }
  }
  //Step backwards through the closed list, from child to parent, and add to the final vector.
  std::vector<navPoint*> reversed;
  reversed.push_back(closedList.back().m_node);
  auto cur = closedList.back();
  while(cur.m_node != _start)
  {
    navPoint * toSearch = cur.m_parent;
    for(size_t i = 0; i < closedList.size(); ++i)
    {
      if(closedList[i].m_node == toSearch)
      {
        cur = closedList[i];
        reversed.push_back(cur.m_node);
        break;
      }
    }
  }

  reverse(reversed.begin(), reversed.end());

  for(auto &i : reversed)
  {
    _a->addWaypoint(i->m_pos);
  }

  return _a->getWaypoints();
}
