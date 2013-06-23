#ifndef POINTTREE_H_
#define POINTTREE_H_

#include "cut/geom/Concepts.hpp"
#include "Geometry.hpp"
#include <kdtree++/kdtree.hpp>
#include "boost/range.hpp"
#include "boost/foreach.hpp"

namespace kdt = KDTree;

static inline Coord_t acc_point(Point item, int k) {
  return item[k];
}

template<
  typename TpointNode,
  typename Taccessor
>
class PointTreeImpl : public kdt::KDTree<2, TpointNode, Taccessor> {
private:
  typedef TpointNode _PointNode;
  typedef kdt::KDTree<2, TpointNode, Taccessor> _Base;
public:
  PointTreeImpl() :
    _Base(std::ptr_fun(acc_point))
  {}

  void insert(const Point& point) {
    _Base::insert(point);
  }

  std::pair<typename _Base::const_iterator, typename _Base::distance_type> find_nearest(const Point& point) {
    return _Base::find_nearest(point);
  }
};

typedef PointTreeImpl<
    Point,
    std::pointer_to_binary_function<Point, int, int32_t>
> PointTree;

#endif