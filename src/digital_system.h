#ifndef __DIGITAL_SYSTEM_H__
#define __DIGITAL_SYSTEM_H__

#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "anti_sound_default_config.h"
#include <math.h>

// Controller
//   - add_speaker
//   - add_mic
//   - mic_data
//   - speaker_sink

// Digital Antisound system

template <class controller_t, class config_t> class DigitalSystem;
template <class config_t> class FakeAudioOutputDevice;


template <class controller_t, class config_t=AntiSoundDefaultConfig>
class DigitalSystem {
  controller_t& controller;

  std::vector<FakeAudioOutputDevice<config_t>*> speakers;

  public:
  boost::function<double(double)> gen_evaluator(int index);
  double evaluate_speaker(int index, double time);
  DigitalSystem(controller_t& ctrlr);
  template <class spkr_t> void add_speaker(spkr_t& spkr);
};

template <class config_t=AntiSoundDefaultConfig>
class FakeAudioOutputDevice {
  template <class controller_t, class ds_config_t>
  friend class DigitalSystem;
  Speaker<config_t>& speaker;
  double *buffer;
  int buffer_size;

  FakeAudioOutputDevice(Speaker<config_t>& spkr) :
    buffer_size(0),
    speaker(spkr),
    buffer(new double[config_t::buffer_size()])
    {}
  ~FakeAudioOutputDevice() { delete buffer; }
};

template <class controller_t, class config_t>
boost::function<double(double)> DigitalSystem<controller_t, config_t>::gen_evaluator(int index) {
  return boost::bind(&DigitalSystem<controller_t, config_t>::evaluate_speaker, this, index, _1);
}

template <class controller_t, class config_t>
double DigitalSystem<controller_t, config_t>::evaluate_speaker(int index, double time) {
  return sin(time*880); // For testing only
}

template <class controller_t, class config_t>
DigitalSystem<controller_t, config_t>::DigitalSystem(controller_t& ctrlr) : controller(ctrlr) {}

template <class controller_t, class config_t>
template <class spkr_t>
void DigitalSystem<controller_t, config_t>::add_speaker(spkr_t& spkr) {
  spkr.evaluator = gen_evaluator(speakers.size());
  speakers.push_back(new FakeAudioOutputDevice<config_t>(spkr));
}


#endif
