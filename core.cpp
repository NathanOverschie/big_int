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

    bool dint::abslst(const dint &a, const dint &b)
    {
        if (a.size() < b.size())
        {
            return true;
        }
        if (a.size() > b.size())
        {
            return false;
        }

        auto pa = a.data.crbegin();
        auto pb = b.data.crbegin();

        for (; pa != a.data.crend(); pa++, pb++)
        {
            if (*pa < *pb)
            {
                return true;
            }
            if (*pa > *pb)
            {
                return false;
            }
        }

        return false;
    }

    bool dint::absgrt(const dint &a, const dint &b)
    {
        if (a.size() > b.size())
        {
            return true;
        }
        if (a.size() < b.size())
        {
            return false;
        }

        auto pa = a.data.crbegin();
        auto pb = b.data.crbegin();

        for (; pa != a.data.crend(); pa++, pb++)
        {
            if (*pa > *pb)
            {
                return true;
            }
            if (*pa < *pb)
            {
                return false;
            }
        }

        return false;
    }

    bool operator>(const dint &a, const dint &b)
    {
        if (a.neg() && b.neg())
        {
            return dint::abslst(a, b);
        }
        if (a.neg())
        {
            return false;
        }
        if (b.neg())
        {
            return true;
        }
        return dint::absgrt(a, b);
    }

    bool operator<(const dint &a, const dint &b)
    {
        if (a.neg() && b.neg())
        {
            return dint::absgrt(a, b);
        }
        if (a.neg())
        {
            return true;
        }
        if (b.neg())
        {
            return false;
        }
        return dint::abslst(a, b);
    }

    bool operator==(const dint &a, const dint &b)
    {
        if (a.size() != b.size())
        {
            return false;
        }

        for (int i = a.size() - 1; i >= 0; i--)
        {
            if (a.data[i] != b.data[i])
            {
                return false;
            }
        }
        return true;
    }

    //TODO make into iterators
    void dint::remove_leading_zeros()
	{
		size_t i;
		for (i = data.size() - 1; i > 1; i--)
		{
			if (data[i] != 0)
			{
				break;
			}
		}

		data.resize(i + 1);
	}

    /**
     * @brief Give a hex string representation of dint
     *
     * @return hex string representations
     */
    //TODO make into iterators
    string dint::toHexString() const
    {
        ostringstream r{};

        r << (negative ? '-' : ' ');

        for (size_t i = size(); i != 0; i--)
        {
            r << hex << setw(sizeof(base) * 2) << setfill('0');
            r << static_cast<unsigned long long>(data[i - 1]);
            r << ' ';
        }

        return r.str();
    }

}
