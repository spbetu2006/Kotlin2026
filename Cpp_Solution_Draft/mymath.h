#pragma once

#include "FixedPoint.h"

#include <vector>
#include <cassert>

#include <iostream>

namespace nuke {

 long long cpow( long long base,  long long exp) {
     long long res = 1;
    for ( long long i = 0; i < exp; ++i) {
        res *= base;
    }
    return res;
}

size_t my_circ_cent(Point p1, Point p2, size_t R, std::vector<FixedPoint> &out) throw(std::runtime_error) {

    out.clear();
    out.reserve(2);

    assert(p1 != p2);

    FixedNumber qr = cpow(p1.x-p2.x, 2) + cpow(p1.y - p2.y, 2);
    FixedNumber x3 = (p1.x+p2.x)/2; 
    FixedNumber y3 = (p1.y+p2.y)/2;
    FixedNumber R2 = R*R;
    if (qr > R2) {
        return 0;
    }
    if (qr == R2) {
        out.emplace_back(x3, y3);
        return 1; 
    }
    FixedNumber q = sqrt(qr);
    FixedNumber delta = R2 - qr/4;    
    delta = sqrt(delta);
    FixedNumber delta_x = (delta*(p1.y-p2.y))/q;
    FixedNumber delta_y = (delta*(p2.x-p1.x))/q;
    out.emplace_back(x3 + delta_x, y3 + delta_y); 
    out.emplace_back(x3 - delta_x, y3 - delta_y);

    return 2;
}

}
