#include <iostream>
#include <fstream>
#include <memory>
#include "shader.hpp"

shader::shader(const std::string &_vert, const std::string &_frag)
{
  GLuint vertShaderId = createShaderFromTextFile(_vert, 1);
  GLuint fragShaderId = createShaderFromTextFile(_frag, 0);

  printInfoLog(DebugMode::COMPILE, vertShaderId);
  printInfoLog(DebugMode::COMPILE, fragShaderId);

  m_id = glCreateProgram();
  glAttachShader(m_id, vertShaderId);
  glAttachShader(m_id, fragShaderId);
  glLinkProgram(m_id);
  glUseProgram(m_id);
}

shader::~shader()
{

}

void shader::printInfoLog(DebugMode _mode, GLuint _shaderId)
{
  GLint infoLogLength = 0;
  GLint charsWritten = 0;
  char * infoLog;
  glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
  if(infoLogLength > 0)
  {
    std::unique_ptr<char []>infoLog(new char [infoLogLength]);
    glGetShaderInfoLog(_shaderId, infoLogLength, &charsWritten, infoLog.get());
    std::cerr << "Log: \n" << infoLog.get() << "\n";
  }

  GLenum mode;
  if(_mode == DebugMode::COMPILE)
  {
    mode = GL_COMPILE_STATUS;
  }
  else if(_mode == DebugMode::LINK)
  {
    mode = GL_LINK_STATUS;
  }
  glGetShaderiv(_shaderId, mode, &infoLogLength);
  if(infoLogLength == GL_FALSE)
  {
   std::cerr << "Shader compilation or linking error\n";
   exit(EXIT_FAILURE);
  }
}

GLuint shader::createShaderFromTextFile(std::string path, int mode)
{
  std::ifstream shaderSrc(path.c_str());
  if(!shaderSrc.is_open())
  {
    std::cerr << "File not found " << path << "\n";
    exit(EXIT_FAILURE);
  }

  std::string source = std::string{
                      (std::istreambuf_iterator<char>(shaderSrc)),
                      (std::istreambuf_iterator<char>())
                      };
  //This was causing an error, get the escape codes right!
  source += '\0';

  GLuint id;

  if(mode) id = glCreateShader(GL_VERTEX_SHADER);
  else id = glCreateShader(GL_FRAGMENT_SHADER);

  const char * cSource = source.c_str();
  glShaderSource(id, 1, &cSource, NULL);
  glCompileShader(id);
  return id;

}
