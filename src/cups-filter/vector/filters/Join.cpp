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
#include "util/Logger.h"
#include "Join.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"

/**
 * Recursive function
 */
static void find_connected(set<Edge*> &occupied, Polyline *polyline, Edge *current)
{

  occupied.insert(current);

  for (Vertex::iterator it = (*current)[1].begin(); it != (*current)[1].end(); it++) {
    Edge *candidate = *it;
    if (candidate == current || occupied.find(candidate) != occupied.end()) {
      continue;
    }
    if (&(*candidate)[0] != &(*current)[1]) {
      candidate->setEnd(candidate->start());
      candidate->setStart(current->end());
    }
    polyline->add(candidate);
    find_connected(occupied, polyline, candidate);
  }
}

/*
 * Iterate over all free edges and collect connected ones into polylines.
 */
void Join::filter(Cut *cut)
{
  LOG_INFO_STR("Join");

  set<Edge*> occupied;

  Mesh mesh = cut->getMesh();
  for (Mesh::iterator it = mesh.begin(); it != mesh.end(); it++) {
    Edge *edge = *it;

    Polyline *polyline = new Polyline();

    if (occupied.find(edge) == occupied.end()) {
      polyline->add(edge);
      find_connected(occupied, polyline, edge);
      cut->add(polyline);
    }
  }

  for (Cut::iterator it = cut->begin(); it != cut->end(); it++) {
    Polyline *p = *it;
    for (Polyline::iterator it_e = p->begin(); it_e != p->end(); it_e++) {
      mesh.remove(*it_e);
    }
  }
}
