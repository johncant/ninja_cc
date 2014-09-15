#ifndef __ABSTRACT_SOUND_SOURCE_H__
#define __ABSTRACT_SOUND_SOURCE_H__

#include "point3d.h"

class AbstractSoundSource {

  public:

  virtual double evaluate(double time, const Point3D &pt) = 0;

};

#endif
