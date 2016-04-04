#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.hpp"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1f;

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in that case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0.0f;
  m_spinYFace=0.0f;
  setTitle("Qt5 Simple NGL Demo");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  // now set the camera size values as the screen size has changed
  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_cam.setShape(45.0f,(float)_w/_h,0.05f,350.0f);
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
}

void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::instance();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // now to load the shader and set the values
  // grab an instance of shader manager

  // we are creating a shader called Phong

  createShaderProgram("diffuse", "diffuseVertex", "diffuseFragment");
  createShaderProgram("colour", "colourVertex", "colourFragment");

  // the shader will use the currently active material and light0 so set them
  ngl::Material m(ngl::STDMAT::GOLD);
  // load our material values to the shader into the structure material (see Vertex shader)
  m.loadToShader("material");
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,20,20);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  // now load to our new camera
  m_cam.set(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(45.0f,(float)720.0/576.0f,0.05f,350.0f);

  m_terrain = new ngl::Obj("resources/models/terrain_01.obj");
  m_terrain->createVAO();

  // get the VBO instance and draw the built in teapot
  ngl::VAOPrimitives * prim=ngl::VAOPrimitives::instance();
  prim->createCapsule("capsule", 0.2f, 0.4f, 20);
  prim->createSphere("sphere", 0.1f, 8);

  constructNavCloud();

  glGenVertexArrays(1, &m_navConnectionsVAO);
  glBindVertexArray(m_navConnectionsVAO);

  std::vector<ngl::Vec3> pts;
  for(auto &np : *m_sim.getNavPoints())
  {
    for(auto &npc : np.m_neighbours)
    {
      vec3 npp = np.getPos();
      vec3 npcp = npc->getPos();

      pts.push_back(ngl::Vec3(npp.m_x, npp.m_y, npp.m_z));
      pts.push_back(ngl::Vec3(npcp.m_x, npcp.m_y, npcp.m_z));
    }
  }

  m_navConnectionsSize = pts.size();

  //Generate a VBO
  GLuint conBuff;
  glGenBuffers(1, &conBuff);
  glBindBuffer(GL_ARRAY_BUFFER, conBuff);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(ngl::Vec3) * pts.size(),
               &pts[0].m_x,
      GL_STATIC_DRAW
      );

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, conBuff);
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
}

void NGLScene::createShaderProgram(const std::string _name, const std::string _vert, const std::string _frag)
{
  ngl::ShaderLib * shader = ngl::ShaderLib::instance();
  shader->createShaderProgram(_name);
  shader->attachShader(_vert, ngl::ShaderType::VERTEX);
  shader->attachShader(_frag, ngl::ShaderType::FRAGMENT);

  shader->loadShaderSource(_vert, "shaders/" + _vert + ".glsl");
  shader->loadShaderSource(_frag, "shaders/" + _frag + ".glsl");

  shader->compileShader(_vert);
  shader->compileShader(_frag);

  shader->attachShaderToProgram(_name, _vert);
  shader->attachShaderToProgram(_name, _frag);

  shader->linkProgramObject(_name);
}

void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib * shader = ngl::ShaderLib::instance();
  ngl::Mat4 MVP;
  ngl::Mat4 M;
  M = m_transform.getMatrix()* m_mouseGlobalTX;
  MVP = M * m_cam.getVPMatrix();
  shader->setShaderParamFromMat4("MVP",MVP);
}

void NGLScene::paintGL()
{
  glViewport(0,0,m_width,m_height);
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ngl::ShaderLib * shader = ngl::ShaderLib::instance();
  shader->use("diffuse");
  shader->setRegisteredUniform("inColour", ngl::Vec4(0.1f, 0.1f, 0.1f, 1.0f));

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  // get the VBO instance and draw the built in teapot
  ngl::VAOPrimitives * prim=ngl::VAOPrimitives::instance();
  // draw
  loadMatricesToShader();
  //prim->draw("capsule");

  shader->setRegisteredUniform("inColour", ngl::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
  m_terrain->draw();

  shader->setRegisteredUniform("inColour", ngl::Vec4(0.8f, 0.03f, 0.03f, 1.0f));
  for(auto &i : *m_sim.getNavPoints())
  {
    m_transform.setPosition(i.m_pos.m_x, i.m_pos.m_y, i.m_pos.m_z);
    loadMatricesToShader();
    prim->draw("sphere");
    m_transform.reset();
  }

  shader->use("colour");
  shader->setRegisteredUniform("inColour", ngl::Vec4(0.8f, 0.03f, 0.03f, 1.0f));
  glBindVertexArray(m_navConnectionsVAO);

  loadMatricesToShader();

  glDrawArraysEXT(GL_LINES, 0, m_navConnectionsSize);
  glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    update();

  }
  // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();

  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if(_event->delta() > 0)
  {
    m_modelPos.m_z+=ZOOM;
  }
  else if(_event->delta() <0 )
  {
    m_modelPos.m_z-=ZOOM;
  }
  update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // that method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quit
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
    // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
    // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
    // show full screen
  case Qt::Key_F : showFullScreen(); break;
    // show windowed
  case Qt::Key_N : showNormal(); break;
  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
  update();
}

void NGLScene::constructNavCloud()
{
  std::vector<ngl::Vec3> verts = m_terrain->getVertexList();
  for(auto &i : verts)
  {
    navPoint pt = {{i.m_x, i.m_y, i.m_z},{}, 1.0f};
    m_sim.addPoint(pt);
  }

  m_sim.generateNavConnections(20.0f);
}
