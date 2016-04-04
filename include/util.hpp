#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>
#include "AABB.hpp"

float fastInvSqrt(float _val);

template<typename tt>
bool inRange(tt val, tt low, tt high)
{
  return val > low and val < high;
}
#endif
