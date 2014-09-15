#ifndef __ANTISOUND_H__
#define __ANTISOUND_H__

#include "sonic_medium.h"

class AntiSound {
  public:

  class Microphone {
    public:
    Microphone(const Point3D& pos) {
    }
  };

  class Speaker {
    public:
    Speaker(const Point3D& pos) {
    }
  };

  private:
  SonicMedium& medium;
  std::vector<Speaker> speakers;
  std::vector<Microphone> microphones;

  public:
  AntiSound(SonicMedium &sm) : medium(sm) { }

  void add_microphone(const Microphone& mic) {
    microphones.push_back(mic);
  }

  void add_speaker(const Speaker& spkr) {
    speakers.push_back(spkr);
  }
};

#endif
