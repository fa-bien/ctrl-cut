/*
 * Segment.h
 *
 *  Created on: Dec 26, 2011
 *      Author: elchaschab
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include "Geometry.h"
#include "config/SegmentSettings.h"

class CutModel;

class Segment: public SegmentSettings {
public:
  Point first;
  Point second;

  Segment();
  Segment(CutModel& parent);
  Segment(const Point&  first, const Point&  second, CutModel& parent);
  Segment(const Point&  first, const Point&  second, const Segment& seg);

  const Point& operator[](size_t idx) const {
    assert(idx < 2);
    if (idx == 0)
      return first;
    else
      return second;
  }

  Point& operator[](size_t idx) {
    assert(idx < 2);
    if (idx == 0)
      return first;
    else
      return second;
  }

  bool operator<(const Segment& other) const {
      return  (this->first < other.first) || ((this->first == other.first) && (this->second < other.second));
  }

  bool operator==(const Segment& other) const {
    return this->first == other.first && this->second == other.second;
  }

  bool operator!=(const Segment& other) const {
    return ! (*this == other);
  }

  const Box& getBox() const {
    return *this->box;
  }

  const Sphere& getSphere() const {
    return *this->sphere;
  }

  /*!
    Calculates the distance from the Point to the infinite segment
  */
  float distance(const Point& p) const;
  /*!
    Returns angle to the positive Y axis
  */
  float getSlope(bool invert = false) const;

  CutModel& getParent() const {
    assert(hasParent());
    return *parent;
  }

  bool hasParent() const {
    return parent != NULL;
  }
private:
  CutModel* parent;
  Box* box;
  Sphere* sphere;

  void make_box_and_sphere(const Point&  first, const Point&  second) {
    if(first < second)
      this->box = new Box(first, second);
    else
      this->box = new Box(second, first);

    Point& diff = this->box->max_corner - this->box->min_corner;
    Point center = (* new Point(this->box->min_corner.x + (diff.x / 2), this->box->min_corner.y + (diff.y / 2)));
    uint32_t radius = boost::math::hypot((diff.x / 2), (diff.y / 2));
    this->sphere = new Sphere(center, radius);
  }
};

typedef std::list<Segment> SegmentList;

enum intersection_result { ALIGN_NONE, ALIGN_INTERSECT, ALIGN_COINCIDENCE, ALIGN_PARALLEL };

/*!
  Calculates alignment of two segments to each other and sets the supplied point to the resulting point if they intersect
  Tip intersections are reported as ordinary intersections.
*/
inline intersection_result intersects(const Segment& s1, const Segment&s2, Point& intersection)
{
  float denom =
    ((s2[1][1] - s2[0][1]) * (s1[1][0] - s1[0][0])) -
    ((s2[1][0] - s2[0][0]) * (s1[1][1] - s1[0][1]));

  float nume_a =
    ((s2[1][0] - s2[0][0]) * (s1[0][1] - s2[0][1])) -
    ((s2[1][1] - s2[0][1]) * (s1[0][0] - s2[0][0]));

  float nume_b =
    ((s1[1][0] - s1[0][0]) * (s1[0][1] - s2[0][1])) -
    ((s1[1][1] - s1[0][1]) * (s1[0][0] - s2[0][0]));

  if (denom == 0.0f) {
    if (nume_a == 0.0f && nume_b == 0.0f)
      return ALIGN_COINCIDENCE;
    else
      return ALIGN_PARALLEL;
  }

  float ua = nume_a / denom;
  float ub = nume_b / denom;

  if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
    intersection.x = s1[0][0] + ua * (s1[1][0] - s1[0][0]);
    intersection.y = s1[0][1] + ua * (s1[1][1] - s1[0][1]);

    return ALIGN_INTERSECT;
  }

  return ALIGN_NONE;
}
inline std::ostream& operator<<(std::ostream &os, const Point& p)  {
  os << "<point x=\"" << p.x << "\" y=\"" << p.y << "\" key=\"" << p.x << "/" << p.y << "\" />";
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const Segment& segment) {
  os << "<segment>" << std::endl;
  os << "  " << segment.first << std::endl;
  os << "  " << segment.second << std::endl;
  os << "</segment>";
  return os;
}

void translate(SegmentList& segments, SegmentList::iterator first, SegmentList::iterator last, const Point& translation);

#endif /* SEGMENT_H_ */