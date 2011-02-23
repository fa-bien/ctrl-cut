#ifndef EDGE_H_
#define EDGE_H_

#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>

class Edge {
public:
  Edge(class Vertex *start, Vertex *end, int power=0, int speed=0, int frequency=0)
    : power(power), speed(speed), frequency(frequency)
  {
    v[0] = start;
    v[1] = end;
    this->id = Edge::cnt++;
  }
  virtual ~Edge() {}

  Vertex *start() const { return v[0]; }
  Vertex *end() const { return v[1]; }
  void setStart(Vertex *vtx) { v[0] = vtx; }
  void setEnd(Vertex *vtx) { v[1] = vtx; }
  const Vertex &operator[](size_t idx) const { return *v[idx]; }
  int power;
  int speed;
  int frequency;

  float getSlope(bool invert=false);
  void invertDirection();
  Vertex *intersects(const Edge &other) const;
  float distance(const Vertex &v) const;
  void detach();

private:
  static int cnt;
  int id;

  Vertex *v[2];

  friend std::ostream &operator<<(std::ostream &os, const Edge &e);
};


#endif
