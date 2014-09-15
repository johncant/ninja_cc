#include <iostream>
#include "point3d.h"
#include "anti_sound.h"
#include "point_sound_source.h"
#include "still_animator.h"
#include "sine_generator.h"
#include "waves_visual.h"
#include "visualizer.h"

int main(int argc, char** argv) {

  SonicMedium medium;

  StillAnimator animator(Point3D(-3.0, 0.0, 0.0));
  SineGenerator generator(1.0, 0, 440);

  PointSoundSource<StillAnimator, SineGenerator> sound_source(medium, animator, generator);

  AntiSound anc(medium);

  AntiSound::Microphone mic0(Point3D(0.0, 0.0, 0.0)),
                        mic1(Point3D(0.0, -0.5, 0.0)),
                        mic2(Point3D(0.0,  0.5, 0.0));

  AntiSound::Speaker spkr0(Point3D(0.1, 0.0, 0.0)),
                     spkr1(Point3D(0.1, -0.5, 0.0)),
                     spkr2(Point3D(0.1,  0.5, 0.0));

  anc.add_microphone(mic0);
  anc.add_microphone(mic1);
  anc.add_microphone(mic2);

  std::cout << "AntiSound initialized" << std::endl;

  std::cout << medium.evaluate(10, Point3D(2.0, 0.0, 0.0)) << std::endl;

  WavesVisual wv(medium);
  Visualizer v(wv);

  v.mainloop();
}
