#include "dint.h"

namespace bigint
{
	namespace
	{
		inline base hi(base x)
		{
			return x >> (bits_per_word / 2);
		}

		inline base lo(base x)
		{
			return ((1ULL << (bits_per_word / 2)) - 1) & x;
		}

		inline void overflow_product(base a, base b, base &out_lo, base &out_hi)
		{
			base s0, s1, s2, s3;

			base x = lo(a) * lo(b);
			s0 = lo(x);

			x = hi(a) * lo(b) + hi(x);
			s1 = lo(x);
			s2 = hi(x);

			x = s1 + lo(a) * hi(b);
			s1 = lo(x);

			x = s2 + hi(a) * hi(b) + hi(x);
			s2 = lo(x);
			s3 = hi(x);

			out_lo = (s1 << bits_per_word / 2) | s0;
			out_hi = (s3 << bits_per_word / 2) | s2;
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
		void basicmult(const container &&a, const container &&b, container &dest)
		{
			size_t sa = a.size();
			size_t sb = b.size();
			for (auto &&i : dest)
			{
				i = 0;
			}

			base c, t;
			base lo, hi;

			for (size_t i = 0; i < sa; i++)
			{
				c = 0;

				for (size_t j = 0; j < sb; j++)
				{
					// Seperate the carry from the remainder
					overflow_product(a[i], b[j], lo, hi);

					t = lo + c;

					dest[i + j] += t;
					// Carry from the addition
					if (t < lo)
						c = 1;
					else
						c = 0;

					// Carry from the addition
					if (dest[i + j] < t)
						c++;

					// Carry from the multiplication
					c += hi;
				}
				dest[i + sb] = c;
			}
		}

		base basicmult(container::iterator &&begin, container::iterator &&end, base x)
		{
			base c, t;
			base lo, hi;

			c = 0;
			for (auto p = begin; p != end; ++p)
			{
				// Seperate the carry from the remainder
				overflow_product(*p, x, lo, hi);

				// Remainder
				t = lo + c;

				*p = t;
				// Carry from the addition
				if (t < lo)
					c = 1;
				else
					c = 0;

				// Carry from the addition
				if (*p < t)
					c++;

				// Carry from the multiplication
				c += hi;
			}

			return c;
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
	 * @pre{n % 2 == 0}
	 * @pre{dest does not overlap with a and b}
	 */
	void dint::karatsuba(container::iterator &&a_begin, container::iterator &&a_end, container::iterator &&b_begin, container::iterator &&b_end, container::iterator dest_0, container::iterator dest_4, size_t n)
	{
		// In this function I will use the notation '<<' and '>>' to mean shift word left and shift word right
		// And the notation thing[i] means the i'th word of thing
		// And the notation a : i means that a is i words long

		if (n == 1)
		{
			overflow_product(*a_begin, *b_begin, *dest_0, *(dest_0 + 1));
			return;
		}

		if (n == 2)
		{
			// Base case: simple multiplication
			// a : 2
			// b : 2
			// dest : 4

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

		if (n % 2 == 1)
		{
			// a = a_big << 1 + a_small
			// b = b_big << 1 + b_small
			// a : n
			// b : n
			// a_big : n - 1 (even)
			// a_small : 1
			// b_big : n - 1 (even)
			// b_small : 1

			// a * b = a_big * b_big << 2 + (a_big * b_small + a_small * b_big) << 1 + a_small * b_small

			base lo, hi;

			base a_s = *a_begin;
			base b_s = *b_begin;

			overflow_product(a_s, b_s, lo, hi);
			// a_small * b_small -> [lo, hi] : 2

			a_s += basicmult(move(a_begin + 1), move(a_end), *b_begin);
			// a_big * b_small -> a : n

			b_s += basicmult(move(b_begin + 1), move(b_end), *a_begin);
			// b_big * a_small -> b : n

			karatsuba(a_begin + 1, move(a_end), b_begin + 1, move(b_end), dest_0 + 2, dest_4, n - 1);
			// a_big * b_big -> dest[2..]

			*dest_0 = lo;
			*(dest_0 + 1) = hi;
			// a_small * b_small -> dest[0, 1]

			// a_big * b_big << 2 + a_small * b_small -> dest

			// Add a_(big * b_small + a_small * b_big) << 1 to dest
			additer(dest_0 + 1, dest_4, a_begin, a_end, dest_0 + 1, dest_4, false);
			additer(dest_0 + 1, dest_4, b_begin, b_end, dest_0 + 1, dest_4, false);
			// a_big * b_big << 2 + (a_big * b_small + a_small * b_big) << 1 + a_small * b_small -> dest

			return;
		}

		// a = a_hi << n/2 + a_lo
		// b = b_hi << n/2 + b_lo
		// a : n
		// b : n
		// a_hi : n/2
		// a_lo : n/2
		// b_hi : n/2
		// b_lo : n/2

		// z2 = a_hi * b_hi : n
		// z1 = a_hi * b_lo + a_hi * b_lo = (a_hi + a_lo) * (b_hi + b_lo) - z2 - z0 : n + 1 (one extra for optional carry)
		// z0 = a_lo * b_lo : n

		// a * b = z2 << n + z1 << n/2 + z0

		// buff[i] = [dest_begin + (i * n/2), dest_begin + (i * n/2) + n/2]
		auto dest_1 = dest_0 + n / 2;
		auto dest_2 = dest_1 + n / 2;
		auto dest_3 = dest_2 + n / 2;

		// buff[i] = [buff_0 + (i * n/2), buff_0 + (i * n/2) + n/2]
		auto a_mid = a_begin + n / 2;
		auto b_mid = b_begin + n / 2;

		// Calc z2
		karatsuba(move(a_mid), move(a_end), move(b_mid), move(b_end), dest_2, dest_3, n / 2);
		// z2 = a_hi * b_hi -> dest[n..] : n

		// Calc z0
		karatsuba(move(a_begin), move(a_mid), move(b_begin), move(b_mid), dest_0, dest_1, n / 2);
		// z0 = a_lo * b_lo -> buff[..n] : n

		// z2 << n + z0 -> dest

		// @todo I dont care about the carries here
		additer(move(a_mid), move(a_end), move(a_begin), move(a_mid), a_mid, a_end, false);
		// a_hi + a_lo -> a[..n/2] : n/2

		additer(move(b_mid), move(b_end), move(b_begin), move(b_mid), a_begin, a_mid, false);
		// b_hi + b_lo -> a[n/2..] : n/2

		karatsuba(move(a_mid), move(a_end), move(a_begin), move(a_end), b_begin, b_end, n / 2);
		// (a_hi * a_lo) * (b_hi * b_lo) -> b : n

		// Substract z0 and z2 from (a_hi * a_lo) * (b_hi * b_lo) to get z1
		subiter(b_begin, b_end, dest_2, dest_4, b_begin, b_end, false);
		subiter(b_begin, b_end, dest_0, dest_2, b_begin, b_end, false);
		// z1 -> b : n

		// Add z1 to dest[n/2..3n/2]
		additer(dest_1, dest_3, b_begin, b_end, dest_1, dest_3, false);

		// z2 << n + z1 << n/2 + z0 -> dest

		return;
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
	 * @pre{dest.size() == a.size() + b.size()}
	 */
	void dint::mult(container::iterator &&a_begin, container::iterator &&a_end, container::iterator &&b_begin, container::iterator &&b_end, container::iterator dest_begin, container::iterator dest_end)
	{
		size_t n = a_end - a_begin;
		size_t m = b_end - b_begin;

		if (n == m)
		{
			karatsuba(move(a_begin), move(a_end), move(b_begin), move(b_end), dest_begin, dest_end, n);
			return;
		}

		if (n < m)
		{
			swap(a_begin, b_begin);
			swap(a_end, b_end);
			swap(n, m);
		}

		// a = a_hi << m + a_lo
		// b = b_lo
		// a * b = a_hi * b_lo << m + a_lo * b_lo

		auto a_mid = a_begin + m;
		auto dest_mid = dest_begin + 2 * m;

		karatsuba(move(a_begin), move(a_mid), move(b_begin), move(b_end), dest_begin, dest_mid, m);
		// a_lo * b_lo -> a[..2m] : 2m

		mult(move(a_mid), move(a_end), move(b_begin), move(b_end), a_begin, a_end);
		// a_hi * b_lo -> dest[m..] : n

		// Add together
		additer(dest_mid, dest_end, a_begin, a_end, dest_mid, dest_end, false);
		// a_hi * b_lo << m + a_lo * b_lo -> dest
	}

	void dint::mult(const container &&a, const container &&b, container &dest)
	{
		container at{a};
		container bt{b};

		mult(move(at.begin()), move(at.end()), move(bt.begin()), move(bt.end()), dest.begin(), dest.end());
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

	void dint::operator*=(const dint &a)
	{
		*this = (*this * a);
	}

	dint operator*(const dint &a, base b)
	{
		dint t{a};
		t.operator*=(b);
		return t;
	}
}