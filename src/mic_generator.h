#ifndef __MIC_GENERATOR__
#define __MIC_GENERATOR__

#include <portaudio.h>
#include <vector>
#include <iostream>
#include "point_sound_source.h"
#include "still_animator.h"

class MicGenerator;



class MicGenerator {
  PaStream *stream;
  std::vector<float> all_sound_values;

//  static bool pa_initialized;

  public:
  static void initialize();
  void write(const float* data, int n);
  MicGenerator();

  double evaluate(double time);
};



#endif
