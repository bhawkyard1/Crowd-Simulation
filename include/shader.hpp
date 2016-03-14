#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <ngl/Types.h>

class shader
{
public:
  shader( const std::string &_vert,
          const std::string &_frag);
  ~shader();
  GLuint getID() const {return m_id;}
private:
  enum class DebugMode{COMPILE, LINK};
  void printInfoLog(DebugMode _mode, GLuint _shaderId);
  GLuint createShaderFromTextFile(std::string, int);
  GLuint m_id;
};

#endif
