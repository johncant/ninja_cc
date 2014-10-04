#ifndef __SPEAKER_H__
#define __SPEAKER_H__

#include <math.h>

template <class config_t = AntiSoundDefaultConfig>
class Speaker : public AbstractSoundSource {
  const Point3D &pos;

  public:
  boost::function<double(double)> evaluator;

  Speaker(const Point3D& pt) : pos(pt) {
  }

  double evaluate(double time, const Point3D &pt) {
    double dist = boost::geometry::distance(pt, pos);

    return evaluate_raw(time - dist/config_t::sound_speed())/pow(dist, 2);
  }

  double evaluate_raw(double time) {
    if (evaluator) {
      //std::cout << evaluator << " Evaluating at " << time << std::endl;
      return evaluator(time);
    } else {
      std::cout << "No evaluator present" << std::endl;
      return 0;
    }
  }

  bool is_fixed_point() { return false; }
  Point3D position() { return pos; }
};

#endif
