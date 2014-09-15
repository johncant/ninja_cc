#ifndef __SINE_GENERATOR_H__
#define __SINE_GENERATOR_H__

#include <math.h>

class SineGenerator {

  double amplitude, frequency, phase;

  public:
  SineGenerator(double amp, double freq, double phi) : amplitude(amp), frequency(freq), phase(phi) {
  }

  double evaluate(double time) {
    return amplitude*sin(2*M_PI*frequency*time + phase);
  }
};

#endif
