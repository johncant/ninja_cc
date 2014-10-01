#include <iostream>
#include "point3d.h"
#include "anti_sound.h"
#include "point_sound_source.h"
#include "still_animator.h"
#include "sine_generator.h"
#include "waves_visual.h"
#include "visualizer.h"
#include "digital_system.h"
#include "controller.h"
#include <random>

int main(int argc, char** argv) {

  SonicMedium medium;

  std::default_random_engine generator;

  std::uniform_real_distribution<double> distribution_x(-20.0,20.0);
  std::uniform_real_distribution<double> distribution_y(-10.0,0.0);
  std::uniform_real_distribution<double> distribution_z(-1.0,1.0);

  StillAnimator animator0(Point3D(-8.0, 3.8864, 0.0));
  StillAnimator animator1(Point3D(-8.0, -3.8864, 0.0));
  SineGenerator g0(1.0, 440, 0);
  SineGenerator g1(1.0, 440, 0);

  PointSoundSource<StillAnimator, SineGenerator> sound_source0(medium, animator0, g0);
  PointSoundSource<StillAnimator, SineGenerator> sound_source1(medium, animator1, g1);

//  for (int i=0; i<= 20; i++) {
//    StillAnimator random_animator(Point3D(
//      distribution_x(generator),
//      distribution_y(generator),
//      distribution_z(generator)
//    ));
//    SineGenerator g0(0.25, 55*pow(2, double(i)/4.0), 0);
//    PointSoundSource<StillAnimator, SineGenerator> sound_source0(medium, random_animator, g0);
//  }

  DigitalSystem<Controller>::Controller c;
  DigitalSystem<Controller> ds(c);
  AntiSound<DigitalSystem<Controller> > anc(medium, ds);

  Microphone mic0(Point3D(0.0, 0.0, 0.0)),
                        mic1(Point3D(0.0, -1.0, 0.0)),
                        mic2(Point3D(0.0, -2.0, 0.0)),
                        mic3(Point3D(0.0, -3.0, 0.0)),
                        mic4(Point3D(0.0, -4.0, 0.0)),
                        mic5(Point3D(0.0, -5.0, 0.0)),
                        mic6(Point3D(0.0, -6.0, 0.0)),
                        mic7(Point3D(0.0, -7.0, 0.0)),
                        mic8(Point3D(0.0,  1.0, 0.0)),
                        mic9(Point3D(0.0,  2.0, 0.0)),
                        mic10(Point3D(0.0,  3.0, 0.0)),
                        mic11(Point3D(0.0,  4.0, 0.0)),
                        mic12(Point3D(0.0,  5.0, 0.0)),
                        mic13(Point3D(0.0,  6.0, 0.0)),
                        mic14(Point3D(0.0,  7.0, 0.0));

  Speaker<AntiSoundDefaultConfig> spkr0(Point3D(1.0, 0.0, 0.0)),
                     spkr1(Point3D(1.0, -1.0, 0.0)),
                     spkr2(Point3D(1.0, -2.0, 0.0)),
                     spkr3(Point3D(1.0, -3.0, 0.0)),
                     spkr4(Point3D(1.0, -4.0, 0.0)),
                     spkr5(Point3D(1.0, -5.0, 0.0)),
                     spkr6(Point3D(1.0, -6.0, 0.0)),
                     spkr7(Point3D(1.0, -7.0, 0.0)),
                     spkr8(Point3D(1.0,  1.0, 0.0)),
                     spkr9(Point3D(1.0,  2.0, 0.0)),
                     spkr10(Point3D(1.0,  3.0, 0.0)),
                     spkr11(Point3D(1.0,  4.0, 0.0)),
                     spkr12(Point3D(1.0,  5.0, 0.0)),
                     spkr13(Point3D(1.0,  6.0, 0.0)),
                     spkr14(Point3D(1.0,  7.0, 0.0));

  anc.add_microphone(mic0);
  anc.add_microphone(mic1);
  anc.add_microphone(mic2);
  anc.add_microphone(mic3);
  anc.add_microphone(mic4);
  anc.add_microphone(mic5);
  anc.add_microphone(mic6);
  anc.add_microphone(mic7);
  anc.add_microphone(mic8);
  anc.add_microphone(mic9);
  anc.add_microphone(mic10);
  anc.add_microphone(mic11);
  anc.add_microphone(mic12);
  anc.add_microphone(mic13);
  anc.add_microphone(mic14);

  anc.add_speaker(spkr0);
  anc.add_speaker(spkr1);
  anc.add_speaker(spkr2);
  anc.add_speaker(spkr3);
  anc.add_speaker(spkr4);
  anc.add_speaker(spkr5);
  anc.add_speaker(spkr6);
  anc.add_speaker(spkr7);
  anc.add_speaker(spkr8);
  anc.add_speaker(spkr9);
  anc.add_speaker(spkr10);
  anc.add_speaker(spkr11);
  anc.add_speaker(spkr12);
  anc.add_speaker(spkr13);
  anc.add_speaker(spkr14);


  std::cout << "AntiSound initialized" << std::endl;

  std::cout << medium.evaluate(10, Point3D(2.0, 0.0, 0.0)) << std::endl;

  WavesVisual wv(medium);

  wv.show_radius(20);

  Visualizer v(wv);

  v.mainloop();
}
