#pragma once

#include "InSequence.h"
#include "FixedNumber.h"
#include "FixedPoint.h"

#include "mymath.h"

#include <functional>
#include <iostream>

namespace nuke {

typedef std::function<bool (const Point &pt)> TTargetVisitorFunc; 

class TargetVisitor {

public:

   TargetVisitor(const InSequence& _seq) : in_seq(_seq) {}

   void enumerateTargets(TTargetVisitorFunc func, long long count = -1) {

       for (InSequence::TPointIter it_c1 = in_seq.m_vct.begin(); it_c1 != in_seq.m_vct.end() && count; ++it_c1) {
       
                   if (!func(*it_c1)) {
                       return;
                   }
                   --count;
       }
   }

protected:

   const InSequence &in_seq;

private:

};

}
