#ifndef __SPEAKER_H__
#define __SPEAKER_H__

template <class config_t = AntiSoundDefaultConfig>
class Speaker : public AbstractSoundSource {
  const Point3D &pos;

  public:
  boost::function<double(double)> evaluator;

  Speaker(const Point3D& pt) : pos(pt) {
  }

  double evaluate(double time, const Point3D &pt) {
    double dist = boost::geometry::distance(pt, pos);

    return evaluate_raw(time - dist/config_t::sound_speed());
  }

  double evaluate_raw(double time) {
    if (evaluator) {
      return evaluator(time);
    } else {
      return 0;
    }
  }

  bool is_fixed_point() { return false; }
  Point3D position() { return pos; }
};

#endif
