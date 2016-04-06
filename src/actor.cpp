#include "actor.hpp"

actor::actor(vec3 _pos)
{
  setPos(_pos);
  m_index = 0;
}

void actor::update(float _dt)
{
  if(m_waypoints.size() > 0)
  {
    float dist = magns(getPos() - m_waypoints[m_index]);
    if(dist < 1.0f)
    {
      if(m_index < m_waypoints.size() - 1)
      {
        m_index++;
        setVel(unit(m_waypoints[m_index] - getPos()));
      }
      else if(m_index == m_waypoints.size() - 1)
      {
        setVel({0.0f, 0.0f, 0.0f});
      }
    }
  }
  updatePos(_dt);
}
