#include <iostream>
#include <ciso646>

#include <QtGui/QGuiApplication>
#include "NGLScene.hpp"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h> //library with shapes i

#include "scene.hpp"
#include "shader.hpp"
#include "sim_time.hpp"

void initializeGL();
void renderGL();

int main(int argc, char * argv [])
{
  //Create the simulation.
  //scene sim;

  QGuiApplication app(argc, argv);
  // create an OpenGL format specifier
  QSurfaceFormat format;
  // set the number of samples for multisampling
  // will need to enable glEnable(GL_MULTISAMPLE); once we have a context
  format.setSamples(4);
  #if defined( __APPLE__)
    // at present mac osx Mountain Lion only supports GL3.2
    // the new mavericks will have GL 4.x so can change
    format.setMajorVersion(4);
    format.setMinorVersion(1);
  #else
    // with luck we have the latest GL version so set to that
    format.setMajorVersion(4);
    format.setMinorVersion(5);
  #endif
  // now we are going to set to CoreProfile OpenGL so we can't use and old Immediate mode GL
  format.setProfile(QSurfaceFormat::CoreProfile);
  // now set the depth buffer to 24 bits
  format.setDepthBufferSize(24);
  // set that as the default format for all windows
  QSurfaceFormat::setDefaultFormat(format);

  // now we are going to create our scene window
  NGLScene window;

  // we can now query the version to see if it worked
  std::cout<<"Profile is "<<format.majorVersion()<<" "<<format.minorVersion()<<"\n";
  // set the window size
  window.resize(1024, 720);
  // and finally show
  window.show();

  return app.exec();
}
