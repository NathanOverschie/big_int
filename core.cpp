#include "dint.h"

namespace bigint
{
    const dint &Nil{container{{}}};

    dint::dint(unsigned long long arg) : data{}
    {
        int words = sizeof(unsigned long long) / sizeof(base);

        base t;

        for (size_t i = 0; i < words && arg != 0; i++)
        {
            data.push_back(static_cast<base>(arg));
            arg >>= (8 * sizeof(base));
        }

        remove_leading_zeros();
    }

    dint::dint(const container &arg) : data{arg}
    {
        remove_leading_zeros();
    }

    dint::dint(container &&arg) : data{arg}
    {
        remove_leading_zeros();
    }

    bool dint::operator!=(const dint &a) const
    {
        return !operator==(*this, a);
    }

    bool dint::operator<=(const dint &a) const
    {
        return !operator>(*this, a);
    }

    bool dint::operator>=(const dint &a) const
    {
        return !operator<(*this, a);
    }

    void dint::operator-=(const dint &a)
    {
        negative = !negative;
        operator+=(a);
        negative = !negative;
    }

    dint dint::operator++(int)
    {
        dint tmp{*this};
        operator++();
        return tmp;
    }

    dint dint::operator--(int)
    {
        dint tmp{*this};
        operator--();
        return tmp;
    }

    dint dint::operator-() const
    {
        dint res{*this};
        res.negative = !res.negative;
        return res;
    }

    size_t dint::size() const
    {
        return data.size();
    }

    base dint::front() const
    {
        return data.front();
    }

    base dint::back() const
    {
        return data.back();
    }

    bool dint::neg() const
    {
        return negative;
    }
    
}
