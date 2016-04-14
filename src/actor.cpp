#include <ciso646>

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
        addVel(-getVel() * 0.1f);
        accelerate(unit(m_waypoints[m_index] - getPos()));
        /*std::cout << "POS : " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z <<
                     "VEL : " << getVel().m_x << ", " << getVel().m_y << ", " << getVel().m_z << std::endl;*/
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
    else
    {
      setVel({0.0f, 0.0f, 0.0f});
    }
    updatePos(_dt);
}

void actor::accelerate(vec3 _dir)
{
    //std::cout << "PASS: " << _dir.m_x << ", " << _dir.m_y << ", " << _dir.m_z << std::endl;
    //std::cout << "PRE: " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z << std::endl;
    float drag = clamp(magns(getVel()) / sqr(m_maxSpeed), 0.0f, 1.0f);
    addVel( -getVel() * drag );
    //std::cout << "DRAG: " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z << std::endl;
    addVel(_dir);
    //std::cout << "POST: " << getPos().m_x << ", " << getPos().m_y << ", " << getPos().m_z << std::endl << std::endl;
}
