#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "digital_system_adapter.h"

template <class adapter_t>
class Controller {

  adapter_t* adapter;
  typename adapter_t::ControllerConfig self_config;

  double last_output_time;
  double sin_phase1;
  double sin_phase2;

  public:
  Controller() {
  }

  typename adapter_t::ControllerConfig initialize(adapter_t &dsa) {
    adapter = &dsa;

    typename adapter_t::ControllerConfig c;
    c.mic_samples = 100;
    c.sample_period = 1/44000.0;
    c.timer_period = 100.0*c.sample_period;
    self_config = c;

    sin_phase1 = 0;
    sin_phase2 = 0;

    return c;
  }

  void configure() {
    // Adapter has changed its config, i.e. number of speakers, mics.
  }

  void on_mic_input(int mic, int length, double* data) {
    std::cout << "receiving " << length << " samples from mic" << mic << std::endl;
  }

  void timer(double time) {
    int n_out_samples = floor(time - last_output_time);
    std::vector<float> values(n_out_samples);

    for(std::vector<float>::iterator it = values.begin()
      ; it != values.end()
      ; it++) {
      *it = 0.5*sin(sin_phase1) + 0.5*sin(sin_phase2);
      sin_phase1 += 2*M_PI*435*self_config.sample_period;
      sin_phase2 += 2*M_PI*445*self_config.sample_period;
    }

    for(typename std::vector<typename adapter_t::SpeakerAdapter>::iterator it = adapter->speakers.begin()
      ; it != adapter->speakers.end()
      ; it++) {
      it->output(values);
    }

    last_output_time += double(n_out_samples)*self_config.sample_period;
    std::cout << "Wrote output to speakers" << std::endl;
  }
};

#endif
