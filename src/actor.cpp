#include <ciso646>

#include "common.hpp"
#include "util.hpp"
#include "actor.hpp"

actor::actor(vec3 _pos)
{
  setPos(_pos);
  setVel({0.0f, 0.0f, 0.0f});

  m_index = 0;
  m_maxSpeed = randFloat(0.5f, 2.0f);
  m_stepOffset = randFloat(rad(0.0f), rad(360.0f));

  m_col = {
    randFloat(0.0f, 1.0f),
    randFloat(0.0f, 1.0f),
    randFloat(0.0f, 1.0f),
    1.0f
  };
}

void actor::update(float _dt)
{
  if(m_waypoints.size() > 0)
  {
    accelerate(unit(m_waypoints[m_index].first - getPos()));
    /*std::cout << "POS : " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z <<
                     "VEL : " << getVel().m_x << ", " << getVel().m_y << ", " << getVel().m_z << std::endl;*/
    float dist = magns(getPos() - m_waypoints[m_index].first);
    if(dist < 1.0f)
    {
      if(m_index < m_waypoints.size() - 1)
      {
        m_index++;
        //setVel(unit(m_waypoints[m_index] - getPos()));
      }
      else if(m_index == m_waypoints.size() - 1)
      {
        setVel({0.0f, 0.0f, 0.0f});
      }
    }
  }
  else
  {
    setVel({0.0f, 0.0f, 0.0f});
  }

  updatePos(_dt);

  physUpdate();
}

void actor::accelerate(vec3 _dir)
{
  //std::cout << "PASS: " << _dir.m_x << ", " << _dir.m_y << ", " << _dir.m_z << std::endl;
  //std::cout << "PRE: " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z << std::endl;
  float drag = clamp(magns(getVel()) / sqr(m_maxSpeed), 0.0f, 1.0f);
  vec3 vel = getVel();
  vel.m_y *= 0.1f;
  addVel( -vel * drag );
  //std::cout << "DRAG: " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z << std::endl;
  addVel(_dir);
  //std::cout << "POST: " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z << std::endl << std::endl;
}

void actor::physUpdate()
{
  addVel(g_gravity);
  vec3 point = m_waypoints[m_index].first;
  vec3 normal = m_waypoints[m_index].second;
  vec3 aPos = getPos();

  //std::cout << "normal : " << normal.m_x << ", " << normal.m_y << ", " << normal.m_z << std::endl;

  //float ydepth = (dotProd1(normal, aPos) - (normal.m_x * point.m_x) - (normal.m_z * point.m_z)) / normal.m_y;
  //float ydepth = aPos.m_y - ( normal.m_x * (point.m_x - aPos.m_x) + normal.m_z * (point.m_z - aPos.m_z) ) / normal.m_y;
  float ydepth = dotProd1(normal, (point - aPos));
  std::cout << "ydepth : " << ydepth << std::endl;

  if(ydepth > 0.0f)
  {
    setPos({aPos.m_x, aPos.m_y + ydepth, aPos.m_z});
    vec3 vel = getVel();
    setVel({vel.m_x, 0.0f, vel.m_z});
  }
}
