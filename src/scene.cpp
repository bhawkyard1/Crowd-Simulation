#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include "scene.hpp"

scene::scene() : window("cSim", 0, 0, 500, 500)
{
  window.makeCurrent();

  ngl::Vec3 from(0,1,1);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  // now load to our new camera
  m_cam.set(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(45.0f,(float)720.0/576.0f,0.05f,350.0f);
  //shader->setUniform("viewerPos",m_cam.getEye().toVec3());
  // now create our light that is done after the camera so we can pass the
  // transpose of the projection matrix to the light to do correct eye space
  // transformations
  ngl::Mat4 iv=m_cam.getViewMatrix();
  iv.transpose();
  ngl::Light light(ngl::Vec3(-2,5,2),ngl::Colour(1,1,1,1),ngl::Colour(1,1,1,1),ngl::LightModes::POINTLIGHT );
  light.setTransform(iv);
  // load these values to the shader as well
  //CRASH HERE light.loadToShader("light");
  // as re-size is not explicitly called we need to do that.
  // set the viewport for openGL we need to take into account retina display
}

void scene::update(float dt)
{

}

void scene::draw(float dt)
{
  ngl::VAOPrimitives * cone = ngl::VAOPrimitives::instance();
  cone->ngl::VAOPrimitives::createCone("my_cone", 0.2f, 0.4f, 6, 1);
  ngl::VAOPrimitives::instance()->draw("my_cone");
}
