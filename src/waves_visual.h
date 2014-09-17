#ifndef __WAVES_VISUAL_H__
#define __WAVES_VISUAL_H__

#include "sonic_medium.h"

class WavesVisualImpl;

class WavesVisual {
  WavesVisualImpl* pimpl;
  const SonicMedium& sonic_medium;


  public:
  ~WavesVisual();
  WavesVisual(const SonicMedium& medium);
  void initialize();
  void configure(double w, double h);
  void draw();
};

#endif
