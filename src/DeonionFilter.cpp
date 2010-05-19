#include "DeonionFilter.h"
#include "Primitives.h"
#include "VectorPass.h"
#include <math.h>

DeonionFilter::~DeonionFilter() {
  // TODO Auto-generated destructor stub
}

void walkTheEdge(Polyline* p, OnionSkin* skin, LineSegment* edge, bool cw) {
  set<LineSegment*> connectors = edge->getEnd()->getConnectors();
  set<LineSegment*>::iterator it;
  LineSegment* candidate;
  LineSegment* next_edge = NULL;

  float edge_slope = edge->getSlope(true);
  float candidate_slope;

  float slope_diff;
  float min_slope_diff = 2 * M_PI;

  //TODO resolve double check
  if(p->hasLineSegment(edge))
    skin->addLineSegment(edge);

  p->eraseLineSegment(edge);

  for (it = connectors.begin(); it != connectors.end(); it++) {
    candidate = *it;

    if (candidate == edge || !p->hasLineSegment(candidate))
      continue;

    if (candidate->getStart() != edge->getEnd())
      candidate->invertDirection();

    candidate_slope = candidate->getSlope();

    slope_diff = edge_slope - candidate_slope;
    if(slope_diff < 0)
      slope_diff+=2*M_PI;
    else if(slope_diff > 2*M_PI)
      slope_diff-=2*M_PI;

    if (slope_diff < min_slope_diff) {
      min_slope_diff = slope_diff;
      next_edge = candidate;
    }
  }

  if (next_edge == NULL && cw) {
    edge->invertDirection();
    walkTheEdge(p, skin, edge, !cw);
  } else if (next_edge != NULL) {
    if(!cw)
      cw = true;
    walkTheEdge(p, skin, next_edge, cw);
  }
}

void DeonionFilter::filter(LaserPass *pass)
{
  std::cerr << "Deonion" << std::endl;
  VectorPass *vpass = dynamic_cast<VectorPass*>(pass);
  
  unsigned int i;
  Polyline* p;

  for (i = 0; i < vpass->polylines.size(); i++) {
    p = vpass->polylines.at(i);

    while (p->getSegmentCount() > 0) {
      OnionSkin* s = new OnionSkin();
      walkTheEdge(p, s, p->findEdge(), true);
      vpass->addOnionSkin(s);
    }
  }
}
