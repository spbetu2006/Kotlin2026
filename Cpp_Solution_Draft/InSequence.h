#pragma once

#include "Point.h"

#include <vector>
#include <istream>
#include <string>
#include <sstream>

namespace {

const char sep = ',';

};

namespace nuke {

class CenterVisitor;
class TargetVisitor;

class InSequence {

public:

    friend std::istream& operator>>(std::istream& is, InSequence& seq) {
        seq.m_vct.clear();

        std::string line;
        std::string token;
         char x, y;

        if (is.eof())
           return is;

        while (1) {
           is >> line;
           if (is.eof())
              break;

           std::stringstream ss(line);
            int x, y;

           std::getline(ss, token, sep);
           x = atoi(token.c_str());
           std::getline(ss, token, sep);
           y = atoi(token.c_str());

           seq.m_vct.emplace_back(x, y);
        }    

        return is;
    }

    friend class CenterVisitor;
    friend class TargetVisitor;

    typedef std::vector<Point>::const_iterator TPointIter;

    size_t size() const {
         return m_vct.size();
    }

protected:

    std::vector<Point> m_vct;

private:


};

}
