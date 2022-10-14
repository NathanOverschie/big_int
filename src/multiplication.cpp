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
				// Seperate the carry from the remainder
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

				// Remainder
				t = (s1 << bits_per_word / 2) | s0;

				tt = t + c;

				dest[i + j] += tt;
				// Carry from the addition
				if (tt < t)
					c = 1;
				else
					c = 0;

				// Carry from the addition
				if (dest[i + j] < tt)
					c++;

				// Carry from the multiplication
				c += (s3 << bits_per_word / 2) | s2;
			}
			dest[i + sb] = c;
		}
	}

	base dint::basicmult(container::iterator &&begin, container::iterator &&end, base x)
	{
		base c, t, tt;
		base s0, s1, s2, s3;
		base at, bt;

		c = 0;
		for (auto p = begin; p != end; ++p)
		{

			// Seperate the carry from the remainder
			at = *p;
			bt = x;

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

			// Remainder
			t = (s1 << bits_per_word / 2) | s0;

			tt = t + c;

			*p = tt;
			// Carry from the addition
			if (tt < t)
				c = 1;
			else
				c = 0;

			// Carry from the addition
			if (*p < tt)
				c++;

			// Carry from the multiplication
			c += (s3 << bits_per_word / 2) | s2;
		}

		return c;
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
	 * @pre{n % 2 == 0}
	 * @pre{dest does not overlap with a and b}
	 */
	void dint::karatsuba(container::iterator &&a_begin, container::iterator &&a_end, container::iterator &&b_begin, container::iterator &&b_end, container::iterator dest_0, container::iterator dest_4, size_t n)
	{

		if (n == 2)
		{
			container a{*a_begin, *a_end};
			container b{*b_begin, *b_end};
			container dest(4);

			basicmult(move(a), move(b), dest);

			auto &&p = dest_0;
			for (int i = 0; i < 4; i++, p++)
			{
				*p = dest[i];
			}

			return;
		}
		// buff[i] = [dest_begin + (i * n/2), dest_begin + (i * n/2) + n/2]
		auto dest_1 = dest_0 + n / 2;
		auto dest_2 = dest_1 + n / 2;
		auto dest_3 = dest_2 + n / 2;

		// buff[i] = [buff_0 + (i * n/2), buff_0 + (i * n/2) + n/2]
		auto a_mid = a_begin + n / 2;
		auto b_mid = b_begin + n / 2;

		if (n % 2 == 1)
		{
			n--;
			a_mid = a_begin + 1;
			b_mid = b_begin + 1;

			dest_1 = dest_0 + 1;
			dest_2 = dest_1 + 1;
			dest_3 = dest_2 + n;
		}
		else
		{

			// z2 = a_hi * b_hi -> dest[2, 3]
			karatsuba(move(a_mid), move(a_end), move(b_mid), move(b_end), dest_2, dest_3, n / 2);

			// z0 = a_lo * b_lo -> buff[0, 1]
			karatsuba(move(a_begin), move(a_mid), move(b_begin), move(b_mid), dest_0, dest_1, n / 2);

			// a_hi + a_lo -> a_hi
			additer(move(a_mid), move(a_end), move(a_begin), move(a_mid), a_mid, a_end, false);

			// b_hi + b_lo -> a_lo
			additer(move(b_mid), move(b_end), move(b_begin), move(b_mid), a_begin, a_mid, false);

			// a_hi * a_lo = (a_hi + a_lo) * (b_hi + b_lo) -> b
			karatsuba(move(a_mid), move(a_end), move(a_begin), move(a_end), b_begin, b_end, n / 2);

			// z1:
			//		b -= z2
			subiter(b_begin, b_end, dest_2, dest_4, b_begin, b_end, false);

			//		b -= z0
			subiter(b_begin, b_end, dest_0, dest_2, b_begin, b_end, false);

			// dest[1,2] += z1
			additer(dest_1, dest_3, b_begin, b_end, dest_1, dest_3, false);

			return;
		}
	}

	void dint::mult(const container &&a, const container &&b, container &dest)
	{

		if (a.size() == b.size())
		{
			size_t n = a.size();

			container at{a};
			container bt{a};

			karatsuba(at.begin(), at.end(), bt.begin(), bt.end(), dest.begin(), dest.end(), n);
		}

		basicmult(move(a), move(b), dest);
	}

	dint operator*(const dint &a, const dint &b)
	{
		dint res;
		res.data = container(a.size() + b.size());

		dint::mult(move(a.data), move(b.data), res.data);

		res.remove_leading_zeros();

		return res;
	}

	void dint::operator*=(base x)
	{
		base c = basicmult(data.begin(), data.end(), x);
		if (c != 0)
		{
			data.push_back(c);
		}
	}

	dint operator*(const dint &a, base b)
	{
		dint t{a};
		t.operator*=(b);
		return t;
	}
}