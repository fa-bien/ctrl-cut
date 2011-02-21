/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Mesh.h"

void Mesh::create(int startX, int startY, int endX, int endY, int power, int speed, int frequency){
  this->create(new Vertex(startX, startY), new Vertex(endX, endY), power, speed, frequency);
}

/*
 * Adds the given edge to the vector pass.
 */
void Mesh::create(Vertex *start, Vertex *end, int power, int speed, int frequency)
{
  //filter zero length edges
  if(start->getKey() == end->getKey())
    return;

  // FIXME: Clip against page size
  if (start->getX() < 0 || start->getY() < 0 ||
      end->getX() < 0 || end->getY() < 0) {

    if (start->getX() < 0) start->setX(0);
    if (start->getY() < 0) start->setY(0);
    if (end->getX() < 0) end->setX(0);
    if (end->getY() < 0) end->setY(0);

    // FIXME: The Windows driver subtracts 1 point from the X
    // coordinate of the end of any line segment which is
    // clipped. Strange, but let's follow suit for now.
    end->setX(end->getX()-1);
  }

  Edge *edge = new Edge(start, end, power, speed, frequency);

  start = mapVertex(start);
  edge->start = start;
  start->attach(edge);

  end = mapVertex(end);
  edge->end = end;
  end->attach(edge);

  this->edges.push_back(edge);
}

void Mesh::remove(Edge* e) {
  this->edges.remove(e);
}

Mesh::iterator Mesh::eliminate(Mesh::iterator it_e) {
  (*it_e)->detach();
  return this->edges.erase(it_e);
}

Vertex* Mesh::mapVertex(Vertex *p)
{
  map<string, Vertex* >::iterator it = vertices.find(p->getKey());

  if (it != vertices.end()) {
    return (Vertex *)it->second;
  }

  vertices.insert(pair<string, Vertex*> (p->getKey(), p));
  return p;
}

ostream& operator<< (ostream &os, const Mesh &mesh) {
  os << "<mesh>" << std::endl;
  os << "<edges cnt=\"" << mesh.edges.size() << "\" >" << std::endl;
  for(Mesh::EdgeList::const_iterator it = mesh.edges.begin(); it != mesh.edges.end(); it++) {
    os << **it;
  }
  os << "</edges>" << std::endl;
  const map<string, Vertex* > &vertices = mesh.getVertexMap();

  os << "<vertices cnt=\"" << vertices.size() << "\" >" << std::endl;
  for (map<string, Vertex*>::const_iterator it = vertices.begin(); it != vertices.end(); it++) {
    Vertex* vec = (Vertex*) (*it).second;
    os << *vec;
  }
  os << "</vertices>" << std::endl;
  os << "</mesh>" << std::endl;

  return os;
}
