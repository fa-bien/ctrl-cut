/*
 * EpilogCUPS - A laser cutter CUPS driver
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
#ifndef R_TYPES_H_
#define R_TYPES_H_

/** Number of bytes in the bitmap header. */
#define BITMAP_HEADER_NBYTES (54)

#include <string>
#include <iostream>

using namespace std;

struct Point2D {
  Point2D(int x, int y) : x(x), y(y) { }
  int x, y;
};

template<class T>
class Pixel {
public:
   Pixel(T r, T g, T b) : r(r), g(g), b(b) { }
   T r,g,b;
};

class Rectangle {
public:
  int ul_x, ul_y, lr_x, lr_y, width, height;
  
  Rectangle(int ul_x = 0, int ul_y = 0, int lr_x = 0, int lr_y = 0) {
    this->ul_x = ul_x;
    this->ul_y = ul_y;
    this->lr_x = lr_x;
    this->lr_y = lr_y;
    
    this->width = lr_x - ul_x;
    this->height = lr_y - ul_y;
  }
  
  bool inside(Point2D* m, int tolerance = 0) {
    return (m->x < (lr_x + tolerance) && m->x > (ul_x - tolerance) && m->y
            < (lr_y + tolerance) && m->y > (ul_y - tolerance));
  }
};

#endif /* R_TYPES_H_ */
