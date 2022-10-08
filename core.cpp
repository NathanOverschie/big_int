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

    inline bool dint::operator!=(const dint &a) const
    {
        return !operator==(*this, a);
    }

    inline bool dint::operator<=(const dint &a) const
    {
        return !operator>(*this, a);
    }

    inline bool dint::operator>=(const dint &a) const
    {
        return !operator<(*this, a);
    }

    inline void dint::operator-=(const dint &a)
    {
        negative = !negative;
        operator+=(a);
        negative = !negative;
    }

    inline dint dint::operator++(int)
    {
        dint tmp{*this};
        operator++();
        return tmp;
    }

    inline dint dint::operator--(int)
    {
        dint tmp{*this};
        operator--();
        return tmp;
    }

    inline dint dint::operator-() const
    {
        dint res{*this};
        res.negative = !res.negative;
        return res;
    }

    inline size_t dint::size() const
    {
        return data.size();
    }

    inline base dint::front() const
    {
        return data.front();
    }

    inline base dint::back() const
    {
        return data.back();
    }

    inline bool dint::neg() const
    {
        return negative;
    }
}
