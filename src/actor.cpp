#include <ciso646>

#include "util.hpp"
#include "actor.hpp"

actor::actor(vec3 _pos)
{
    setPos(_pos);
    m_index = 0;
    m_maxSpeed = randFloat(1.00f, 4.00f);
}

void actor::update(float _dt)
{
    if(m_waypoints.size() > 0)
    {
        addVel(-getVel() * 0.1f);
        accelerate(unit(m_waypoints[m_index] - getPos()));
        float dist = magns(getPos() - m_waypoints[m_index]);
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
    updatePos(_dt);
}

void actor::accelerate(vec3 _dir)
{
    float projectedSpeed = magns(getVel() + _dir);

    //If entity will speed up, AND be over max speed, truncate (to max speed).
    if(projectedSpeed > magns(getVel()) and projectedSpeed > sqr(m_maxSpeed))
    {
        projectedSpeed = sqrt(projectedSpeed);
        float toAdd = m_maxSpeed - projectedSpeed;
        addVel(_dir * toAdd);
        //std::cout << "Adding : " << (_dir * toAdd).m_x << ", " << (_dir * toAdd).m_y << ", " << (_dir * toAdd).m_z << " now at " << getVel().m_x << ", " << getVel().m_y << std::endl;
    }
    else
    {
        addVel(_dir);
        //std::cout << "Adding : " << _dir.m_x << ", " << _dir.m_y << ", " << _dir.m_z << " now at " << getVel().m_x << ", " << getVel().m_y << std::endl;
    }
}
