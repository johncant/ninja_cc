#ifndef __ANTISOUND_MICROPHONE_H__
#define __ANTISOUND_MICROPHONE_H__

class Microphone {
  template <class system_t, class config_t>
  friend class AntiSound;
  Point3D pos;

  public:
  boost::function<double(double)> evaluator;

  Microphone(const Point3D& p) : pos(p) {
  }

  double evaluate(double time) {
    if (evaluator) {
      //std::cout << evaluator << " Evaluating at " << time << std::endl;
      return evaluator(time);
    } else {
      std::cout << "No evaluator present" << std::endl;
      return 0;
    }
  }
};



#endif
