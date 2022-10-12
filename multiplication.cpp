#include "dint.h"

namespace bigint
{
	inline base hi(base x)
	{
		return x >> (bits_per_word / 2);
	}

	inline base lo(base x)
	{
		return ((1ULL << (bits_per_word / 2)) - 1) & x;
	}

	/**
	 * @brief
	 *
	 * @param a_begin
	 * @param a_end
	 * @param b_begin
	 * @param b_end
	 * @param dest_begin
	 * @param dest_end
	 * @pre{dest.size = a.size + b.size}
	 */
	void dint::basicmult(const container &&a, const container &&b, container &dest)
	{
		size_t sa = a.size();
		size_t sb = b.size();
		for (auto &&i : dest)
		{
			i = 0;
		}

		base c, t, tt;
		base s0, s1, s2, s3;
		base at, bt;

		for (size_t i = 0; i < sa; i++)
		{
			c = 0;

			for (size_t j = 0; j < sb; j++)
			{
				//Seperate the carry from the remainder
				at = a[i];
				bt = b[j];

				base x = lo(at) * lo(bt);
				s0 = lo(x);

				x = hi(at) * lo(bt) + hi(x);
				s1 = lo(x);
				s2 = hi(x);

				x = s1 + lo(at) * hi(bt);
				s1 = lo(x);

				x = s2 + hi(at) * hi(bt) + hi(x);
				s2 = lo(x);
				s3 = hi(x);

				//Remainder
				t = (s1 << bits_per_word / 2) | s0;

				tt = t + c;

				dest[i+j]+= tt;
				//Carry from the addition
				if(tt < t)
					c = 1;
				else
					c = 0;


				//Carry from the addition
				if(dest[i+j] < tt)
					c++;
				
				//Carry from the multiplication
				c += (s3 << bits_per_word / 2) | s2;


			}
			dest[i + sb] = c;
		}
	}

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
	 * @pre{buff_end - buff_begin == 4n - 1}
	 * @pre{n % 2 == 0}
	 * @pre{buff does not overlap with a b and dest}
	 */
	void dint::karatsuba(container::iterator a_begin, container::iterator a_end, container::iterator b_begin, container::iterator b_end, container::iterator dest_0, container::iterator dest_4, container::iterator buff_0, container::iterator buff_8, size_t n)
	{
		// buff[i] = [dest_begin + (i * n/2), dest_begin + (i * n/2) + n/2]
		auto dest_1 = dest_0 + n / 2;
		auto dest_2 = dest_1 + n / 2;
		auto dest_3 = dest_2 + n / 2;

		// buff[i] = [buff_0 + (i * n/2), buff_0 + (i * n/2) + n/2]
		auto buff_1 = buff_0 + n / 2;
		auto buff_2 = buff_1 + n / 2;
		auto buff_3 = buff_2 + n / 2;
		auto buff_4 = buff_3 + n / 2;
		auto buff_5 = buff_4 + n / 2;
		auto buff_6 = buff_5 + n / 2;
		auto buff_7 = buff_6 + n / 2;

		auto a_mid = a_begin + n / 2;
		auto b_mid = b_begin + n / 2;

		// a_hi + a_lo -> buff[0]
		additer(a_mid, a_end, a_begin, a_mid, buff_0, buff_1, false);

		// b_hi + b_lo -> buf[1]
		additer(b_mid, b_end, b_begin, b_mid, buff_1, buff_2, false);

		// buff[0] * buff[1] = (a_hi + a_lo) * (b_hi + b_lo) -> buff[0,1]
		karatsuba(buff_0, buff_1, buff_1, buff_2, buff_0, buff_2, buff_2, buff_6, n / 2);

		// z2 = a_hi * b_hi -> buff[2, 3]
		karatsuba(a_mid, a_end, b_mid, b_end, buff_2, buff_4, buff_4, buff_8, n / 2);

		// z0 = a_lo * b_lo -> buff[4, 5]
		karatsuba(a_begin, a_mid, b_begin, b_mid, buff_4, buff_6, dest_0, dest_4, n / 2);

		// z1 = buff[0,1] - buff[2,3] - buff[4,5] = (a_hi + a_lo) * (b_hi + b_lo) - (a_hi * b_hi) - (a_lo * b_lo) = a_hi * b_lo + a_lo * b_hi -> dest[1,2]
		//		buff[0,1] -> dest[1,2]
		for (auto &&p = buff_0, &&q = dest_1; q != dest_3; ++p, ++q)
		{
			*q = *p;
		}

		//		dest[1,2] -= buff[2,3]
		subiter(dest_1, dest_3, buff_2, buff_4, dest_1, dest_3, false);

		//		dest[1,2] -= buff[4,5]
		subiter(dest_1, dest_3, buff_4, buff_6, dest_1, dest_3, false);

		// Clear dest[0] and dest[3]
		for (auto &&p = buff_0; p != buff_1; p++)
		{
			*p = 0;
		}
		for (auto &&p = buff_3; p != buff_4; p++)
		{
			*p = 0;
		}

		// dest[0,1] += z0
		additer(dest_0, dest_2, buff_4, buff_6, dest_0, dest_2, false);

		// dest[2,3] += z2
		additer(dest_2, dest_4, buff_2, buff_4, dest_2, dest_4, false);
	}

	void dint::mult(const dint &&a, const dint &&b, dint &dest)
	{
		basicmult(move(a.data), move(b.data), dest.data);
	}

	dint operator*(const dint &a, const dint &b)
	{
		dint res;
		res.data = container(a.size() + b.size());

		dint::mult(move(a), move(b), res);

		res.remove_leading_zeros();

		return res;
	}

}