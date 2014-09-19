#ifndef __ANTISOUND_H__
#define __ANTISOUND_H__

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include "sonic_medium.h"
#include "boost/function.hpp"
#include "anti_sound_default_config.h"

template <class config_t = AntiSoundDefaultConfig>
class Speaker : public AbstractSoundSource {
  const Point3D &pos;

  public:
  boost::function<double(double)> evaluator;

  Speaker(const Point3D& pt) : pos(pt) {
  }

  double evaluate(double time, const Point3D &pt) {
    double dist = boost::geometry::distance(pt, pos);

    if (evaluator) {
      return evaluator(time - dist/config_t::sound_speed());
    } else {
      return 0;
    }
  }

  bool is_fixed_point() { return false; }
  Point3D position() { return pos; }
};

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
