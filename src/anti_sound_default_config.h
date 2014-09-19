#ifndef __ANTI_SOUND_DEFAULT_CONFIG_H__
#define __ANTI_SOUND_DEFAULT_CONFIG_H__

class AntiSoundDefaultConfig {
  public:
  static inline double sample_period() { return 1/44000.0; }
  static inline unsigned int buffer_size() { return 2048; }
  static inline double sound_speed() { return 342.0; }
};

#endif
