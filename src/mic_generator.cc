#include "mic_generator.h"

bool MicGenerator_pa_initialized = false;

int mic_callback(
  const void *input,
  void *output,
  unsigned long frame_count,
  const PaStreamCallbackTimeInfo* time_info,
  PaStreamCallbackFlags status_flags,
  void* that
);

void MicGenerator::initialize() {
  if (MicGenerator_pa_initialized) {
    return;
  }

  PaError err = Pa_Initialize();

  if( err != paNoError ) {
    std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
  } else {
    MicGenerator_pa_initialized = true;
  }
}


void MicGenerator::write(const float* data, int n) {
  // TODO - mutex, resize
  for (int i=0; i<n; i++) {
    all_sound_values.push_back(data[i]*100);
  }
}

MicGenerator::MicGenerator() {
  PaError err;

  initialize();
  if (!MicGenerator_pa_initialized) {
    return;
  }

  err = Pa_OpenDefaultStream(
    &stream,
    1,
    0,
    paFloat32,
    44100,
    256,
    mic_callback,
    this
  );

  if( err != paNoError ) {
    std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    return;
  }

  Pa_StartStream(stream);

  if( err != paNoError ) {
    std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    return;
  }
}


double MicGenerator::evaluate(double time) {
  int sample_index = int(nearbyint(time*44100.0));

  if (!MicGenerator_pa_initialized) {
    return 0.0;
  }

  if (sample_index < 0) {
    return 0.0;
  } else if (sample_index < all_sound_values.size()) {
//    std::cout << "index " << sample_index << " giving " << all_sound_values[sample_index] << std::endl;
    return all_sound_values[sample_index];
  } else {
//    std::cout << "Underflow " << all_sound_values.size() << "  -  " << sample_index << std::endl;
    while (all_sound_values.size() < sample_index) {
      all_sound_values.push_back(0.0);
    }
    return 0;
  }
}

int mic_callback(
  const void *input,
  void *output,
  unsigned long frame_count,
  const PaStreamCallbackTimeInfo* time_info,
  PaStreamCallbackFlags status_flags,
  void* that
) {

//  std::cout << "Receiving from mic: " << frame_count << std::endl;
  ((MicGenerator*)that)->write((float*) input, frame_count);

  return 0;

}
