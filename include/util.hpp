#ifndef UTIL_HPP
#define UTIL_HPP

template<typename tt>
bool inRange(tt val, tt low, tt high)
{
  return val > low and val < high;
}

#endif
