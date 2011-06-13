#ifndef CUTGRAPH_H_
#define CUTGRAPH_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <map>

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "CutEdge.h"
#include "CutVertex.h"
#include "Geometry.h"

using boost::adjacency_list;
using boost::undirectedS;
using boost::vecS;
using boost::add_vertex;
using boost::add_edge;
using std::vector;
using std::map;
using std::pair;

class CutGraph : public adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty> {
public:
  boost::graph_traits<CutGraph>::edges_size_type edge_count;

  typedef typename boost::graph_traits<CutGraph>
    ::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<CutGraph>
    ::edge_descriptor Edge;
  typedef typename boost::graph_traits<CutGraph>
      ::vertices_size_type v_size;

  typedef map<const GeometryMapping, Vertex> GeomVertexMap;

  CutGraph() : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>() , edge_count(0){}
  CutGraph(const CutGraph& graph) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(graph) , edge_count(0) {}
  CutGraph(v_size size) : adjacency_list<vecS, vecS, undirectedS, VertexGeomProperty, EdgeGeomProperty>(size) , edge_count(0){}

  inline const Point* gePoint(const CutGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<0>();
  }

  inline const Segment* getSegment(const CutGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<1>();
  }

  inline const SegmentString* getSegmentString(const CutGraph::Vertex& v) {
    return get(vertex_geom, *this)[v].get<2>();
  }

  inline const Point* gePoint(const CutGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<0>();
  }

  inline const Segment* getSegment(const CutGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<1>();
  }

  inline const SegmentString* getSegmentString(const CutGraph::Edge& e) {
    return get(edge_geom, *this)[e].get<2>();
  }

  inline const SegmentString* setSegmentString(const CutGraph::Edge& e, const SegmentString& string) {
    return get(edge_geom, *this)[e].get<2>() = &string;
  }

  CutGraph::Vertex* findVertex(const GeometryMapping &map);
  CutGraph::Vertex addVertex(const GeometryMapping &map);
  void createEdge(const Segment& seg);
  void createEdges(const SegmentString& string1, const SegmentString& string2);
  void createEdges(const SegmentString& string1, const SegmentString& string2, const CutGraph::Vertex& v_origin);
  static CutGraph& createPlanarGraph(SegmentList::const_iterator start, SegmentList::const_iterator end);
  static CutGraph& createCompleteGraph(StringList::const_iterator start, StringList::const_iterator end);
  static std::pair<CutGraph&, CutGraph::Vertex>& createCompleteGraphFromPoint(const Point& origin, StringList::const_iterator start, StringList::const_iterator end);

private:
  GeomVertexMap geometries;
};

#endif