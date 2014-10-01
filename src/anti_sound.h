#ifndef __ANTISOUND_H__
#define __ANTISOUND_H__

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include "sonic_medium.h"
#include "boost/function.hpp"
#include "anti_sound_default_config.h"
#include "speaker.h"

class Microphone {
  public:
  Microphone(const Point3D& pos) {
  }
};

template <class system_t, class config_t = AntiSoundDefaultConfig>
class AntiSound {

  private:
  SonicMedium& medium;
  system_t & system;

  std::vector<Speaker<config_t> > speakers;
  std::vector<Microphone> microphones;

  public:
  AntiSound(SonicMedium &sm, system_t& sys) : medium(sm), system(sys) { }

  void add_microphone(const Microphone& mic) {
    microphones.push_back(mic);
  }

  void add_speaker(Speaker<config_t>& spkr) {
    speakers.push_back(spkr);
    system.add_speaker(spkr);
    medium.add_sound_source(spkr);
  }
};

#endif
