#include "dint.h"

namespace bigint
{

    /**
     * @brief adds the two dints together
     * 
     * @param big first addition argument
     * @param small second addition argument 
     * @param dest the result will go into this dint
     * @param increment weither or not to do an increment
     * @pre{big >= small}
     * @pre{dest.size() == big.size()}
     * @post{!increment => dest = big + small}
     * @post{increment => dest = big + small + 1}
     */
    void dint::add(const dint &&big, const dint &&small, dint &dest, const bool increment = false)
    {
        // Initialize the iterators
        auto pbig = big.data.begin();
        auto psmall = small.data.begin();
        auto pdest = dest.data.begin();

        base t;
        // Initialize the carry bit (can be one initially)
        base c = increment ? 1 : 0;

        // First part of calculation, both numbers contribute to the result
        for (; psmall != small.data.end(); pbig++, psmall++, pdest++)
        {

            t = *pbig;
            *pdest = t + *psmall + c;
            c = (*pdest >= t + c ? 0 : 1);
        }

        // Second part of the calculation, only one number contributes to the result
        for (; pbig != big.data.end() && (c == 1 || big != dest); pbig++, pdest++)
        {

            *pdest = *pbig + c;
            c = (*pdest >= c ? 0 : 1);
        }

        // Optionally add an extra word to store the leftover carry bit in.
        if (c == 1 && pbig == big.data.end())
        {
            dest.data.push_back(base{1});
        }
    }

    /**
     * @brief substracts b from a.
     * 
     * @param big first addition argument
     * @param small second addition argument 
     * @param dest the result will go into this dint
     * @param increment weither or not to do an increment
     * @pre{big >= small}
     * @pre{dest.size() == big.size()}
     * @post{!increment => dest = big - small}
     * @post{increment => dest = big - small - 1}
     */
    void dint::sub(const dint &&big, const dint &&small, dint &dest, const bool increment = false)
    {
        // Initialize the iterators
        auto pbig = big.data.begin();
        auto psmall = small.data.begin();
        auto pdest = dest.data.begin();

        // Iterator for checking leading zeros for substraction
        container::iterator pzeros;
        bool zeros = false;

        base t;
        // Initialize the carry bit (can be one initially)
        base c = increment ? 1 : 0;

        // First part of calculation, both numbers contribute to the result
        for (; psmall != small.data.end(); pbig++, psmall++, pdest++)
        {
            t = *pbig;
            *pdest = *pbig - *psmall - c;
            c = (*pdest <= t ? 0 : 1);

            if (*pdest == 0)
            {
                if (!zeros)
                {
                    pzeros = pdest;
                    zeros = true;
                }
            }
            else
            {
                zeros = false;
            }
        }

        // Second part of the calculation, only one number contributes to the result
        for (; pbig != big.data.end() && (c == 1 || big != dest); pbig++, pdest++)
        {
            t = *pbig;
            *pdest = t - c;
            c = (*pdest <= t ? 0 : 1);

            if (*pdest == 0)
            {
                if (!zeros)
                {
                    pzeros = pdest;
                    zeros = true;
                }
            }
            else
            {
                zeros = false;
            }
        }

        // Remove leading zeros
        if (zeros)
        {
            dest.data.erase(pzeros, dest.data.end());
        }
    }

    /**
     * @brief prefix ++ operator
     * 
     * @return dint& incremented dint
     */
    dint &dint::operator++()
    {
        if (negative)
        {
            sub(move(*this), move(Nil), *this, true);
        }
        else
        {
            add(move(*this), move(Nil), *this, true);
        }
        return *this;
    }

    /**
     * @brief prefix -- operator
     * 
     * @return dint& decremented dint
     */
    dint &dint::operator--()
    {
        if (!negative)
        {
            sub(move(*this), move(Nil), *this, true);
        }
        else
        {
            add(move(*this), move(Nil), *this, true);
        }
        return *this;
        return *this;
    }

    /**
     * @brief postfix ++ operator
     * 
     * @return dint 
     */
    dint dint::operator++(int)
    {
        dint tmp{*this};
        operator++();
        return tmp;
    }

    /**
     * @brief postfix -- operator
     * 
     * @return dint 
     */
    dint dint::operator--(int)
    {
        dint tmp{*this};
        operator--();
        return tmp;
    }

    /**
     * @brief addition of two dints
     * 
     * @param a 
     * @param b 
     * @return dint 
     */
    dint operator+(const dint &a, const dint &b)
    {

        size_t sa = a.data.size();
        size_t sb = b.data.size();

        size_t max = sa > sb ? sa : sb;

        dint res;

        // The result vector has the size of the biggest number (possibly 1 extra word for the carry but thats done later)
        res.data = container(max);

        if (a.negative == b.negative)
        {
            // Addition
            dint::add(move(a), move(b), res);
        }
        else
        {
            // Substraction
            if (a > b)
            {
                dint::sub(move(a), move(b), res);
                res.negative = a.negative;
            }
            else
            {
                dint::sub(move(b), move(a), res);
                res.negative = b.negative;
            }
        }

        return res;
    }

    /**
     * @brief addition of two dints
     * 
     * @param a 
     * @param b 
     * @return dint 
     */
    dint operator+(const dint &a, dint &&b)
    {
        b.operator+=(a);
        return b;
    }

    /**
     * @brief substraction
     * 
     * @param a 
     * @param b 
     * @return dint 
     */
    dint operator-(const dint &a, const dint &b)
    {
        return a + (-b);
    }

    /**
     * @brief substraction
     * 
     * @param a 
     * @param b 
     * @return dint 
     */
    dint operator-(const dint &a, dint &&b)
    {
        b -= a;
        return b;
    }

    void dint::operator+=(base x)
    {

        bool n = false;

        size_t s = size();

        if (negative)
        {
            x = -x;
            negative = false;
            n = true;
        }

        data[0] += x;
        char c = (data[0] >= x) ? 0 : 1;

        size_t i;

        for (i = 1; i < s && c == 1; i++)
        {
            data[i] += c;
            c = data[i] >= c ? 0 : 1;
        }

        if (c == 1 && i == s)
        {
            data.push_back(1);
        }

        if (n)
        {
            negative = true;
        }
    }

    void dint::operator+=(const dint &a)
    {
        // get the sizes
        size_t sa = a.data.size();
        size_t sb = data.size();

        // resize the current number if neccesary
        if (sb < sa)
        {
            data.resize(sa);
        }

        if (a.negative == negative)
        {
            // Addition
            add(move(*this), move(a), *this);
        }
        else
        {
            // Substraction
            if (absgrt(a, *this))
            {
                sub(move(a), move(*this), *this);
                negative = a.negative;
            }
            else
            {
                sub(move(*this), move(a), *this);
            }
        }
    }

    void dint::operator-=(const dint &a)
    {
        negative = !negative;
        operator+=(a);
        negative = !negative;
    }

    void dint::operator-=(base a)
    {
        negative = !negative;
        operator+=(a);
        negative = !negative;
    }
}