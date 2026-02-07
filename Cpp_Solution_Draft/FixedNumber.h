#pragma once

#include <stdexcept>
#include <cmath>

static_assert(sizeof(long long) == 8, "Unsupported CPU arch");

#include "mymath_tables.h"

namespace {

 long long cpow1( long long base, size_t exp) {
    long long res = 1;
    for (size_t i{}; i < exp; ++i) {
        res *= base;
    }
    return res;
}

/*
** NB: база и основание подобраны для решения конкретной задачи "nuke" с заданными
**     входными ограничениями (координаты 0..99 и радиус не более 71)
*/
const  long long pow2base =     cpow1(2, 48);  
const  long long pow2halfBase = cpow1(2, 24);

const char * g_strDivError = "Division overflow";

};

namespace nuke {

class FixedNumber {

public:

    FixedNumber( long long up = 0) : m_val(up*pow2base), m_precise(true) {}

    FixedNumber( long long up,  long long down) : m_val(up*pow2base + down)
    , m_precise(true) {}

    FixedNumber(const FixedNumber &obj) : m_val(obj.m_val), m_precise(obj.m_precise) {}

    friend std::ostream& operator<<(std::ostream& os, const FixedNumber& object) {
        long long val = object.m_val;
        if (val < 0) {
            os << "-";
            val = -val;
        }
        os << (long long)(val / pow2base);
        long long below = val % pow2base;
        if (below)
            os << ".";
        while (below) {
            below *= 10;
            os << below / pow2base;
            below %= pow2base;
        }
        return os;
    }

    FixedNumber&
    operator = ( long long up) {
        m_val = up*pow2base;
        m_precise = true;
        return *this;
    }

    FixedNumber&
    operator = (const FixedNumber &obj) {
        m_val = obj.m_val;
        m_precise = obj.m_precise;
        return *this;
    }

    /*
    ** isPrecise:
    **   == true => признак, что число отображается точно с фиксированной точностью
    **   == false => признак, что число отображается приблизительно (+- fp_epsilon)
    **
    ** операции с FixedNumber, которые могут привести к потери точности:
    **   - извлечения корня
    **   - деление с остатком
    **   - другие операции
    **
    ** операции с FixedNumber, которые не могут привести к потери точности:
    **   - сложение
    **   - вычитание
    **   - умножение
    **   - деление без остатка
    **   - возведение в натуральную степень
    **
    ** NB! Потеря точности в процессе арифметических операция является необратимой
    **     Соответственно в дальнейшем возможна поправка на fp_epsilon
    */

    bool isPrecise() const {
        return m_precise;
    }

    friend FixedNumber sqrt(const FixedNumber& in) {
        FixedNumber num_sqrt(0, ( long long)(sqrtl(in.m_val)*pow2halfBase) );
        /* Определяем потерю точности */
        num_sqrt.m_precise = in.m_precise;
        if (in.m_precise) {
            if (in.m_val % pow2base) {
                num_sqrt.m_precise = false;
            } else {
                if (!t_squares.count(in.m_val / pow2base)) {
                    num_sqrt.m_precise = false;
                }
            }
        }
        return num_sqrt;
    }

    friend bool operator == (const FixedNumber& one, const FixedNumber& two) {
        return (one.m_val == two.m_val);
    }

    friend bool operator == (const FixedNumber& one,  long long two) {
        return (one.m_val / pow2base) == two;
    }

    friend bool operator == ( long long one, const FixedNumber& two) {
        return one = (two.m_val / pow2base);
    }

    friend bool operator > (const FixedNumber& one, const FixedNumber& two) {
        return (one.m_val > two.m_val);
    }

    friend bool operator > (const FixedNumber& one,  long long two) {
        return one.m_val > (two * pow2base);
    }

    friend bool operator < (const FixedNumber& one, const FixedNumber& two) {
        return (one.m_val < two.m_val);
    }

    friend bool operator < (const FixedNumber& one,  long long two) {
        return one.m_val < (two * pow2base);
    }

   /* C++20: generate all operators automatically! */

    friend bool operator >= (const FixedNumber& one, const FixedNumber& two) {
        return (one.m_val >= two.m_val);
    }

    friend bool operator >= (const FixedNumber& one,  long long two) {
        return one.m_val >= (two * pow2base);
    }

    friend bool operator <= (const FixedNumber& one, const FixedNumber& two) {
        return (one.m_val <= two.m_val);
    }

    friend bool operator <= (const FixedNumber& one,  long long two) {
        return one.m_val <= (two * pow2base);
    }

    /* ... */

    friend FixedNumber operator / (const FixedNumber& one, const FixedNumber& two) {
        if (!two.m_val) {
            throw std::runtime_error(g_strDivError);
        } 
        return FixedNumber(0, ((long double)one.m_val*(long double)pow2base) / two.m_val);
    }

    friend FixedNumber operator / (const FixedNumber& one,  long long two) {
        if (!two) {
            throw std::runtime_error(g_strDivError);
        }
        return FixedNumber(0, one.m_val / two);
    }
 
    friend FixedNumber operator / ( long long one, const FixedNumber& two) {
        if (!two.m_val) {
            throw std::runtime_error(g_strDivError);
        }
        return FixedNumber(0, ((long double)pow2base*(long double)pow2base*(long double)one) / two.m_val);
    }

    /* ... */

    friend FixedNumber operator - (const FixedNumber& one, const FixedNumber& two) {
        return FixedNumber(0, one.m_val - two.m_val);
    }

    /* ... */

    friend FixedNumber operator * (const FixedNumber& one, const FixedNumber& two) {
        return FixedNumber(0, ((long double)one.m_val * (long double)two.m_val) / (long double)pow2base );
    }

    /* ... */
    friend FixedNumber operator + (const FixedNumber& one, const FixedNumber& two) {
        // std::cout << "FixedNumber addition" << std::endl;
        return FixedNumber(0, one.m_val + two.m_val);
    }

     long long getVal() const {
        return m_val;
    }
   
protected:

     long long m_val; 
    bool m_precise;       /* Флаг потери точности */

private:

};

namespace {

    const FixedNumber fp_epsilon = FixedNumber(0, 1);
}

}
