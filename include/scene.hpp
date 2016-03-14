#ifndef SCENE_HPP
#define SCENE_HPP

#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/Light.h>

#include "vectors.hpp"

#include "SDLOpenGL.hpp"

class scene
{
  SDLOpenGL window;
  ngl::Camera m_cam;
public:
  scene();
  void update(float dt);
  void draw(float update);
  void renderSwap() {window.swapWindow();}
};

#endif
