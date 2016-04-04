#include <ciso646>

#include "scene.hpp"

scene::scene()
{

}

void scene::update(float dt)
{

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
