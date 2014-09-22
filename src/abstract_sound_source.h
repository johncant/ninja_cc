#ifndef __ABSTRACT_SOUND_SOURCE_H__
#define __ABSTRACT_SOUND_SOURCE_H__

#include "point3d.h"

class AbstractSoundSource {

  public:

  virtual double evaluate(double time, const Point3D &pt) = 0;
  virtual double evaluate_raw(double time) = 0;

  virtual bool is_fixed_point() = 0;
  virtual Point3D position() = 0;
};

#endif
