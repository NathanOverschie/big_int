#include "dint.h"

namespace bigint
{
    /**
     * @brief
     *
     * @param a_begin
     * @param a_end
     * @param b_begin
     * @param b_end
     * @param dest_begin
     * @param dest_end
     * @param n
     * @pre{(a_end - a_begin) == n - 1}
     * @pre{(b_end - b_begin) == n - 1}
     * @pre{dest_end - dest_begin == 2n - 1}
     */
    void dint::karatsuba(container::iterator a_begin, container::iterator a_end, container::iterator b_begin, container::iterator b_end, container::iterator dest_begin, container::iterator dest_end, size_t n)
    {
        if (n == 1)
        {
            size_t offset = (sizeof(base) * 4);

            base a_lo = *a_begin << offset;
            base a_hi = *a_begin >> offset;

            base b_lo = *b_begin << offset;
            base b_hi = *b_begin >> offset;

            *dest_begin = a_lo * b_lo;
            *dest_end = a_hi * a_hi;

            base t = a_lo * b_hi + a_hi * b_lo;

            *dest_begin += t << offset;
            *dest_end   += t >> offset;
        }

        if (n % 2 == 1)
        {
            n++;
        }

        vector<base> z0(n);
        vector<base> z1(n);
        vector<base> z2(n);

        auto a_mid = a_begin + n / 2;
        auto b_mid = b_begin + n / 2;

        auto dest_mid = dest_begin + n;

        // z0 := Karatsuba(alo , blo , n/2)
        karatsuba(a_begin, a_mid, b_begin, b_mid, z0.begin(), z0.end(), n / 2);

        // z2 := Karatsuba(ahi , bhi , n/2)
        karatsuba(a_mid, a_end, b_mid, b_end, z2.begin(), z2.end(), n / 2);

        // z1 := Karatsuba(ahi + alo , bhi + blo , n/2) − z0 − z2
        vector<base> a(n / 2);
        vector<base> b(n / 2);
        additer(a_mid, a_end, a_begin, a_mid, a.begin(), a.end(), false);
        additer(b_begin, b_mid, b_mid, b_end, b.begin(), b.end(), false);

        karatsuba(a.begin(), a.end(), b.begin(), b.end(), z1.begin(), z1.end(), n / 2);

        subiter(z1.begin(), z1.end(), z0.begin(), z0.end(), z1.begin(), z1.end(), false);
        subiter(z1.begin(), z1.end(), z2.begin(), z2.end(), z1.begin(), z1.end(), false);

        // dest := z2 << n + z1 << n/2 + z0
        additer(z0.begin(), z0.end(), Nil.data.begin(), Nil.data.end(), dest_begin, dest_mid, false);
        additer(z1.begin(), z1.end(), dest_begin + n / 2, dest_mid + n / 2, dest_begin + n / 2, dest_mid + n / 2, false);
        additer(z2.begin(), z2.end(), dest_mid, dest_end, dest_mid, dest_end, false);
    }

    void dint::mult(const dint &&a, const dint &&b, dint &dest)
    {
    }

}