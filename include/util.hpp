#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <vector>
#include "AABB.hpp"

float fastInvSqrt(float _val);
float randFloat(float low, float high);
float sqr(float _arg);

template<typename tt>
bool inRange(tt val, tt low, tt high)
{
  return val > low and val < high;
}

template<typename tt>
tt clamp(tt v, tt m, tt M)
{
    if(v < m) return m;
    else if(v > M) return M;
    return v;
}
#endif
