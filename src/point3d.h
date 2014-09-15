#ifndef __POINT3D_H__
#define __POINT3D_H__

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/core/cs.hpp>

typedef boost::geometry::model::point<
  double,
  3,
  boost::geometry::cs::cartesian
> Point3D;

#endif
