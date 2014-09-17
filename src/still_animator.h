#ifndef __STILL_ANIMATOR_H__
#define __STILL_ANIMATOR_H__

#include <math.h>
#include "point3d.h"
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include "settings.h"


class StillAnimator {

  private:
  const Point3D pos;

  public:
  StillAnimator(const Point3D p) : pos(p) {
  }


  typedef std::vector<boost::tuple<double, Point3D> > sound_origin_t;

  sound_origin_t evaluate_origins(const double time, const Point3D& p) {
    sound_origin_t origins;
    double dist = boost::geometry::distance(pos,p);
    origins.push_back(boost::make_tuple(time - fabs(dist)/Settings::propagation_speed, pos));
    return origins;
  }

  Point3D get_position() {
    return pos;
  }
};

#endif
