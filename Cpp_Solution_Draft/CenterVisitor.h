#pragma once

#include "InSequence.h"
#include "FixedPoint.h"

#include "mymath.h"

#include <functional>

namespace nuke {

typedef std::function<bool (const FixedPoint &pt)> TCenterVisitorFunc; 

class CenterVisitor {

public:

   CenterVisitor(const InSequence& _seq) : in_seq(_seq) {}

   void enumerateCenters(TCenterVisitorFunc func, size_t R, long long count = -1) {

       for (InSequence::TPointIter it_c1 = in_seq.m_vct.begin(); it_c1 != in_seq.m_vct.end() && count; ++it_c1) {
           
           InSequence::TPointIter it_c2 = it_c1; 
           ++it_c2;
  
           for (;it_c2 != in_seq.m_vct.end() && count; ++it_c2) {

               std::vector<FixedPoint> vct_center;
 
               try {
               
                   const size_t cnt = my_circ_cent(*it_c1, *it_c2, R, vct_center); 

                   if (cnt && count) {
                       --count;
                       if (!func(vct_center[0]))
                           return;
                   }

                   if (cnt > 1 && count) {
                       --count;
                       if (!func(vct_center[1]))
                           return;
                   }
               }
               catch (std::runtime_error) {
                   /* ... */
               }
           }
       }
   }

protected:

   const InSequence &in_seq;

private:

};

}
