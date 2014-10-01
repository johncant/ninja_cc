#ifndef __DIGITAL_SYSTEM_H__
#define __DIGITAL_SYSTEM_H__

#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "anti_sound_default_config.h"
#include "digital_system_adapter.h"
#include <math.h>
#include <boost/circular_buffer.hpp>
#include <boost/variant.hpp>

// Controller
//   - add_speaker
//   - add_mic
//   - mic_data
//   - speaker_sink

// Digital Antisound system

struct ControllerConfig {
  int mic_samples;
  double sample_period;
  double timer_period;
};


template <template <class adapter_t> class controller_t, class config_t=AntiSoundDefaultConfig> class DigitalSystem;
template <class config_t> class FakeAudioOutputDevice;


template <template <class adapter_t> class controller_t, class config_t>
class DigitalSystem {

  public:
  class SpeakerAdapter {
    friend class DigitalSystem<controller_t, config_t>;
    std::vector<float> all_sound_values;

    public:
    void output(const std::vector<float>& values) {
      all_sound_values.insert(
        all_sound_values.end(),
        values.begin(),
        values.end()
      );
    }

    SpeakerAdapter(const Speaker<config_t>& spkr) {
    }
  };

  // We pass one of these to the controller
  class Adapter {
    friend class DigitalSystem<controller_t, config_t>;
    public:
    typedef SpeakerAdapter SpeakerAdapter;
    typedef ControllerConfig ControllerConfig;
    std::vector<SpeakerAdapter> speakers;
  };

  typedef controller_t<Adapter> Controller;

  Controller& controller;

  ControllerConfig controller_config;

  Adapter adapter;

  double last_mic_event_time;
  double last_timer_event_time;
  double playback_time;

  public:

  boost::function<double(double)> gen_evaluator(int index);
  double evaluate_speaker(int index, double time);
  DigitalSystem(Controller& ctrlr);
  template <class spkr_t> void add_speaker(spkr_t& spkr);
  void time_advance_to(double to_time);

  private:
  // Used in impl. These can be local in > C++0x:
  struct MicEvent { };
  struct TimerEvent { };
  typedef boost::variant<MicEvent, TimerEvent> EitherEvent;

  struct EventVisitor : public boost::static_visitor<> {

    DigitalSystem<controller_t, config_t> &system;
    double time;
    EventVisitor(DigitalSystem<controller_t, config_t>& sys, double t) : system(sys), time(t) { }

    void operator()(const TimerEvent & timer_event) {
      system.controller.timer(time);
      std::cout << "TimerEvent " << time << std::endl;
    }

    void operator()(const MicEvent & mic_event) {
      std::cout << "MicEvent" << time << std::endl;
    }
  };
};




template <template <class adapter_t> class controller_t, class config_t>
boost::function<double(double)> DigitalSystem<controller_t, config_t>::gen_evaluator(int index) {
  return boost::bind(&DigitalSystem<controller_t, config_t>::evaluate_speaker, this, index, _1);
}

template <template <class adapter_t> class controller_t, class config_t>
void DigitalSystem<controller_t, config_t>::time_advance_to(double to_time) {

  // We now sequence the events
  typedef std::multimap<double, EitherEvent> EventsContainer;
  EventsContainer pending_events;

  std::cout << "Advancing to " << to_time << std::endl;

  for ( double time = last_mic_event_time+controller_config.mic_samples*controller_config.sample_period
      ; time <= to_time
      ; time += controller_config.mic_samples*controller_config.sample_period) {
    pending_events.insert(std::make_pair(time, EitherEvent( MicEvent() ) ) );
  }

  last_mic_event_time += controller_config.mic_samples*controller_config.sample_period;

  for ( double time = last_timer_event_time+controller_config.timer_period
      ; time <= to_time
      ; time += controller_config.timer_period) {
    pending_events.insert(std::make_pair(time, EitherEvent(TimerEvent())));
  }

  last_timer_event_time += controller_config.timer_period;

  // Dig out events in chronological sequence


  for ( typename EventsContainer::iterator it = pending_events.begin()
      ; it != pending_events.end()
      ; it++) {
    EventVisitor event_visitor(*this, it->first);
    boost::apply_visitor(event_visitor, it->second);
  }

  playback_time = to_time;
}

template <template <class adapter_t> class controller_t, class config_t>
double DigitalSystem<controller_t, config_t>::evaluate_speaker(int index, double time) {

  double dt = time - playback_time;
  if (dt > 0.0) time_advance_to(time);

  int sample_index = nearbyint(time/controller_config.sample_period);
  if (sample_index >= 0 && sample_index < adapter.speakers[index].all_sound_values.size()) {
    return adapter.speakers[index].all_sound_values[sample_index];
  } else {
    return 0.0;
  }
}

template <template <class adapter_t> class controller_t, class config_t>
DigitalSystem<controller_t, config_t>::DigitalSystem(Controller& ctrlr)
  : controller(ctrlr) {
  controller_config = controller.initialize(adapter);
  controller.configure();
}

template <template <class adapter_t> class controller_t, class config_t>
template <class spkr_t>
void DigitalSystem<controller_t, config_t>::add_speaker(spkr_t& spkr) {
  spkr.evaluator = gen_evaluator(adapter.speakers.size());
  adapter.speakers.push_back(SpeakerAdapter(spkr));
  controller.configure();
}


#endif
