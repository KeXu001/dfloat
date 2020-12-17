
#include "other_class.h"

other_class::other_class()
  : member(xu::dfloat("0.01"))
{}

xu::dfloat other_class::get() const
{
  return member;
}