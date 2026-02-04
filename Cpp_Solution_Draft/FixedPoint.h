#pragma once

#include "FixedNumber.h"

namespace nuke {

struct FixedPoint {

   FixedNumber x, y;

   FixedPoint() : x(0), y(0) {}

   FixedPoint( long long _x,  long long _y) : x(_x), y(_y) {}

   FixedPoint(FixedNumber _x, FixedNumber _y) : x(_x), y(_y) {}

};

}
