#pragma once

namespace nuke {

struct Point {

    char x, y;

   Point() : x(0), y(0) {}

   Point( char _x,  char _y) : x(_x), y(_y) {}

   friend bool operator == (const Point &one, const Point &two) {
       return (one.x == two.x) && (one.y == two.y);
   }

   friend bool operator != (const Point &one, const Point &two) {
       return !(one == two);
   }

};

}
