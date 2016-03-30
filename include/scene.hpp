#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/Light.h>

#include "actor.hpp"
#include "vectors.hpp"

#include "NGLScene.hpp"

class scene
{
  NGLScene window;
  ngl::Camera m_cam;
  std::vector<actor> m_actors;
  std::vector<size_t> m_partitions;
public:
  scene();
  void update(float dt);
  void broadPhase(vec3 _pos, vec3 _dim, short int lvl);
  void narrowPhase();
  void draw(float update);
};

#endif
