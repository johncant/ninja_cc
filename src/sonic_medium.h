#ifndef __SONIC_MEDIUM_H__
#define __SONIC_MEDIUM_H__


#include "abstract_sound_source.h"
#include <vector>

class SonicMedium {
  public:
  std::vector<AbstractSoundSource*> sound_sources;

  template <class sound_source_t>
  void add_sound_source(sound_source_t& s) {
    sound_sources.push_back((AbstractSoundSource*) &s);
  }

  template <class sound_source_t>
  void remove_sound_source(sound_source_t& s) {
    //sound_sources.push_back(boost::weak_ptr<AbstractSoundSource>::new(&s));
//    throw "Not yet implemented"
  }

  double evaluate(double time, const Point3D &pt) {
    double result = 0e0;
    for(std::vector<AbstractSoundSource*>::iterator it = sound_sources.begin(); it != sound_sources.end(); it++) {
      result += (*it)->evaluate(time, pt);
    }
    return result;
  }

};

#endif
