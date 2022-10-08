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
}
