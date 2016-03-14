#include <iostream>
#include "SDLOpenGL.hpp"
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
  scene sim;

  initializeGL();

  //Timer used to keep track of simulation time.
  //The argument is the fps of the updates, higher = more detailed.
  sim_time clock(120.0f);

  //Is the simulation running?
  bool active = true;

  while(active == true)
  {
    SDL_Event event;
    while(SDL_PollEvent( &event ))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          active = false;
          break;
        case SDL_KEYDOWN :
            switch(event.key.keysym.sym)
            {
              case SDLK_ESCAPE :
                active = false;
                break;
              case SDLK_w :
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
              case SDLK_s :
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
        }
    }

    //Set current time (timer keeps track of time since cur time was last set).
    clock.setCur();

    //Update the game in small time-steps (dependant on the timers fps).
    while(clock.getAcc() > clock.getFrame())
    {
      sim.update(clock.getDiff());
      clock.incrAcc( -clock.getDiff() );
    }

    //Draw the game.
    float diff_clamped = clock.getDiff();
    if(diff_clamped == 0.0f) diff_clamped = 0.01f;
    sim.draw( clock.getAcc() / diff_clamped );

    //renderGL();

    sim.renderSwap();
  }
  SDL_Quit();

  return EXIT_SUCCESS;
}

void initializeGL()
{
   ngl::NGLInit::instance();
   glClearColor(1.0, 0.0, 0.0, 1.0);
   shader color("shaders/colorVertex.glsl", "shaders/colorFragment.glsl");
}

void renderGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
