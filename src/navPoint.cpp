#include <ciso646>
#include "navPoint.hpp"

bool operator ==(const navPoint &lhs, const navPoint &rhs)
{
    return (lhs.m_pos == rhs.m_pos) and (lhs.m_weight == rhs.m_weight);
}

bool lessX(const navPoint * a, const navPoint *b)
{
  return a->m_pos.m_x < b->m_pos.m_x;
}

bool lessY(const navPoint *a, const navPoint *b)
{
  return a->m_pos.m_y < b->m_pos.m_y;
}

bool lessZ(const navPoint *a, const navPoint *b)
{
  return a->m_pos.m_z < b->m_pos.m_z;
}
