#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include "scene.hpp"

scene::scene() : window()
{

}

void scene::update(float dt)
{
  window.update();
}

void scene::draw(float dt)
{
  ngl::VAOPrimitives * cone = ngl::VAOPrimitives::instance();
  cone->ngl::VAOPrimitives::createCone("my_cone", 0.2f, 0.4f, 6, 1);
  ngl::VAOPrimitives::instance()->draw("my_cone");
}

void scene::broadPhase(vec3 _pos, vec3 _dim, short lvl)
{
  for(auto &i : m_actors)
  {

  }
}
