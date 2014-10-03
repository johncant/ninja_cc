#ifndef __DIGITAL_SYSTEM_H__
#define __DIGITAL_SYSTEM_H__

#include <math.h>
#include <boost/circular_buffer.hpp>
#include <boost/variant.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "anti_sound_default_config.h"
#include "digital_system_adapter.h"
#include "microphone.h"

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

  void synchronize_outputs(double time) {
    int already_read_output_count = ceil(time/controller_config.sample_period);
    for( typename std::vector<SpeakerAdapter>::iterator it = adapter.speakers.begin()
       ; it != adapter.speakers.end()
       ; it++) {
      // Fill with zeros. This is like an underflow, and it means that subsequent outputs will not write into the future.
      while (it->all_sound_values.size() < already_read_output_count + 100) {
        it->all_sound_values.push_back(0);
      }
    }
  }

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
  template <class mic_t> void add_microphone(mic_t& mic);

  std::vector<Microphone*> mics;

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

      if (time > system.playback_time) {
        system.synchronize_outputs(system.playback_time);
      }

      std::cout << "TimerEvent " << time << std::endl;
      system.controller.timer(time);
      system.last_timer_event_time = time;

      system.playback_time = time;
    }

    void operator()(const MicEvent & mic_event) {

      if (time > system.playback_time) {
        system.synchronize_outputs(system.playback_time);
      }

      for ( int mic_n=0
          ; mic_n < system.mics.size()
          ; mic_n++) {
        std::vector<float> samples(system.controller_config.mic_samples);
        for (int i=0; i<system.controller_config.mic_samples; i++) {
          double sample_time = time - (system.controller_config.mic_samples - i)*system.controller_config.sample_period;
          samples[i] = system.mics[mic_n]->evaluate(sample_time);
        }

        system.controller.on_mic_input(mic_n, samples);
        samples.clear();
        system.last_mic_event_time = time;


        system.playback_time = time;
      }
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

  for ( double time = last_mic_event_time+controller_config.mic_samples*controller_config.sample_period
      ; time <= to_time
      ; time += controller_config.mic_samples*controller_config.sample_period) {
    pending_events.insert(std::make_pair(time, EitherEvent( MicEvent() ) ) );
  }

  for ( double time = last_timer_event_time+controller_config.timer_period
      ; time <= to_time
      ; time += controller_config.timer_period) {
    pending_events.insert(std::make_pair(time, EitherEvent(TimerEvent())));
  }


  for ( double time = last_timer_event_time+controller_config.timer_period
      ; time <= to_time
      ; time += controller_config.timer_period) {
    pending_events.insert(std::make_pair(time, EitherEvent(TimerEvent())));
  }

  // Dig out events in chronological sequence


  for ( typename EventsContainer::iterator it = pending_events.begin()
      ; it != pending_events.end()
      ; it++) {
    EventVisitor event_visitor(*this, it->first);
    boost::apply_visitor(event_visitor, it->second);
  }

}

template <template <class adapter_t> class controller_t, class config_t>
double DigitalSystem<controller_t, config_t>::evaluate_speaker(int index, double time) {

//  std::cout << "Evaluating speaker" << 
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

// TODO - not really as generic as it looks
template <template <class adapter_t> class controller_t, class config_t>
template <class mic_t>
void DigitalSystem<controller_t, config_t>::add_microphone(mic_t& mic) {
  mics.push_back(&mic);
  controller.configure();
}

#endif
