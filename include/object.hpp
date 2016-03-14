#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include "vectors.hpp"

#include "base.hpp"

class object: public base
{
  vector<vec3> m_points;
public:
  object();
  object(vec3);
  object(vector<vec3>);
  object(vec3, vector<vec3>);

  void define(vector<vec3> _points) {m_points = _points;}
};

#endif
