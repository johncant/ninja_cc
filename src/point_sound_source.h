#ifndef __POINT_SOUND_SOURCE_H__
#define __POINT_SOUND_SOURCE_H__

#include <math.h>
#include <boost/tuple/tuple.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include "abstract_sound_source.h"
#include "sonic_medium.h"
#include "point3d.h"

template <
  class animator_t,
  class generator_t
> class PointSoundSource : public AbstractSoundSource {

  const SonicMedium& medium;
  generator_t& generator;
  animator_t& animator;
  public:

  PointSoundSource(SonicMedium& _sm, animator_t& a, generator_t& g);

  double evaluate(const double time, const Point3D &pt);

  bool is_fixed_point();
  Point3D position();
};

// Impl

template <
  class animator_t,
  class generator_t
> PointSoundSource<animator_t, generator_t>::PointSoundSource(SonicMedium& sm, animator_t& a, generator_t& g) : medium(sm), generator(g), animator(a) {
  sm.add_sound_source(*this);
}

template <
  class animator_t,
  class generator_t
> double PointSoundSource<animator_t, generator_t>::evaluate(const double time, const Point3D &pt) {

  typedef std::vector<boost::tuple<double, Point3D> > sound_origin_t;
  sound_origin_t sound_origins = animator.evaluate_origins(time, pt);
  double sound_value = 0;
  Point3D zero(0, 0, 0);

  for(sound_origin_t::const_iterator it = sound_origins.begin(); it != sound_origins.end(); it++) {

    sound_value += generator.evaluate(it->get<0>())*fmin(1, 1/boost::geometry::distance(it->get<1>(), pt));
  }

  return sound_value;
}

template <
  class animator_t,
  class generator_t
> bool PointSoundSource<animator_t, generator_t>::is_fixed_point() { return true; /* This is a lie, but ok if we're moving much slower than the speed of sound and close to all receivers */}

// Again, this is totally wrong. Just for testing
template <
  class animator_t,
  class generator_t
> Point3D PointSoundSource<animator_t, generator_t>::position() {
  return animator.get_position();
}

#endif
