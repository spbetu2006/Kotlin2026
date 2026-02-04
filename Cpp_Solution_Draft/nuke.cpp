#include "InSequence.h"

#include <fstream>
#include <iostream>
#include <cassert>

#include "CenterVisitor.h"
#include "TargetVisitor.h"

using namespace std;
using namespace nuke;

const size_t maxRadius2 = 71; // Все цели будут поражены наверняка

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cout << "Usage: ./nuke <targets_input> <radius>" << endl; 
        return 1; 
    }

    // Считываем входные данные
    const size_t radius = atoi(argv[2]);
    assert(radius);
    ifstream in(argv[1]);
    if (in.fail())
        return 1;
    InSequence in_seq;
    in >> in_seq;
    assert(in_seq.size());

    // Частный случай: "полное" решение
    if (radius >= maxRadius2) {
        cout << 49 << " "  << 49 << in_seq.size() << endl;
        return 0;
    }

    // Частные случай: "простое" решение
    if (1 == in_seq.size()) {
        TargetVisitor cts(in_seq);
        cts.enumerateTargets([&](const Point& pt) -> bool {
            cout << (int)pt.x << " " << (int)pt.y << " " << 1 << endl;
            return true;
        });
        return 0;
    } 

    /* 
    ** Общий случай (алькуляция решения):
    **     - генерируем всевозможные центры (enumerateCenters)
    **     - для каждого из вариантов считаем количество центров в круге
    **     - находим максимум из предложенных вариантов
    **     - выводим решение
    ** NOTE: используется стандартный N^3 алгоритм
    */
    const  long long radius2 = radius*radius;

    size_t max_count = 0;
    FixedNumber max_x, max_y;

    CenterVisitor cvs(in_seq);
    cvs.enumerateCenters([&](const FixedPoint &pt) -> bool {
        size_t count = 0;        

        TargetVisitor cts(in_seq);
        cts.enumerateTargets([&](const Point& tgt) -> bool {
        
            FixedNumber dist = (pt.x - tgt.x)*(pt.x - tgt.x) + (pt.y - tgt.y)*(pt.y - tgt.y);

            if (dist <= radius2) {
                ++count;
            };
            return true;
        });

        if (count > max_count) {
            max_count = count;
            max_x = pt.x;
            max_y = pt.y;
            if (count == in_seq.size()) {
                /* Лучшее возможное решение найдено: дальне не продолжаем */
                return false;
            }
        }

        return true;
    }, radius);

    if (max_count) {
        cout << max_x << " " << max_y << " " << max_count << endl; 
    } else {
        /* Случай, когда "хорошего" решения не нашлось - выводим любую цель */
        TargetVisitor cts(in_seq);
        cts.enumerateTargets([&](const Point& pt) -> bool {
            cout << (int)pt.x << " " << (int)pt.y << " " << 1 << endl;
            return true;
        }, 1);
        return 0;        
    };

    return 0;
}
