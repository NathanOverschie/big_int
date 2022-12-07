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
	}

	base basicmult(container::iterator p, const container::iterator &end, base x)
	{
		base c, t;
		base lo, hi;

		c = 0;
		for (; p != end; ++p)
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
	 * @pre{(a_end - a_begin) == n}
	 * @pre{(b_end - b_begin) == n}
	 * @pre{dest_end - dest_begin == 2n}
	 * @pre{buff_end - buff_begin == 4n}
	 * @pre{dest does not overlap with a and b}
	 * @pre{dest = 0}
	 */
	void dint::karatsuba(
		const container::iterator &big_begin,
		const container::iterator &big_end,
		const container::iterator &small_begin,
		const container::iterator &small_end,
		container::iterator dest_begin,
		container::iterator dest_end,
		const container::iterator &buff_begin,
		const container::iterator &buff_end,
		size_t n)
	{
		// In this function I will use the notation '<<' and '>>' to mean shift word left and shift word right
		// And the notation thing[i] means the i'th word of thing
		// And the notation a : i means that a is i words long

#if debugprint
		{
			if (!(n == big_end - big_begin))
				throw runtime_error("n isnt right size");
			if (!(n == small_end - small_begin))
				if (!(dest_end - dest_begin == 2 * n))
				{
					throw runtime_error("dest isnt right size");
				}
			if (!(buff_end - buff_begin == 4 * n))
				throw runtime_error("buff isnt right size");

			cout << "a: " << n << " ={";
			for (auto &&p = container::iterator{big_begin}; p != big_end; p++)
			{
				cout << hex << (int)*p << ' ';
			}
			cout << "}" << endl;

			cout << "b: " << n << " ={";
			for (auto &&p = container::iterator{small_begin}; p != small_end; p++)
			{
				cout << hex << (int)*p << ' ';
			}
			cout << "}" << endl;
		}
#endif

		bool b_Nil = true;
		for (auto &&p = container::iterator{small_begin}; p != small_end; ++p)
		{
			if (*p != 0)
			{
				b_Nil = false;
				break;
			}
		}

		if (b_Nil)
		{

			// Clear dest
			for (auto &p = dest_begin; p != dest_end; p++)
			{
				*p = 0;
			}

#if debugprint
			{
				cout << "dest: " << 2 * n << " ={";
				for (auto &p = dest_begin; p != dest_end; p++)
				{
					*p = 0;
					cout << hex << (int)*p << ' ';
				}
				cout << "}" << endl;
			}
#endif

			return;
		}

		if (n == 1)
		{
			// Base case: simple multiplication
			// a : 1
			// b : 1
			// dest : 2
			overflow_product(*big_begin, *small_begin, *dest_begin, *(dest_begin + 1));

#if debugprint
			{
				cout << "dest: " << 2 << " ={";
				for (auto &&p = dest_begin; p != dest_end; p++)
				{
					cout << hex << (int)*p << ' ';
				}
				cout << "}" << endl;
			}
#endif

			return;
		}

		if (n % 2 == 1)
		{
			// ODD n

			// a = a_big << 1 + a_small
			// b = b_big << 1 + b_small
			// a : n
			// b : n
			// a_big : n - 1 (even)
			// a_small : 1
			// b_big : n - 1
			// b_small : 1

			// a * b = a_big * b_big << 2 + (a_big * b_small + a_small * b_big) << 1 + a_small * b_small

			base lo, hi;

			base a_s = *big_begin;
			base b_s = *small_begin;

			overflow_product(a_s, b_s, lo, hi);
			// a_small * b_small -> [lo, hi] : 2

			a_s += basicmult(big_begin + 1, big_end, *small_begin);
			// a_big * b_small -> a : n

			b_s += basicmult(small_begin + 1, small_end, *big_begin);
			// b_big * a_small -> b : m

			karatsuba(big_begin + 1, big_end, small_begin + 1, small_end, dest_begin + 2, dest_end, buff_begin + 4, buff_end, n - 1);
			// a_big * b_big -> dest[2..] : 2n - 2

			*dest_begin = lo;
			*(dest_begin + 1) = hi;
			// a_small * b_small -> dest[0, 1]

			// a_big * b_big << 2 + a_small * b_small -> dest

			// Add a_(big * b_small + a_small * b_big) << 1 to dest
			additer(dest_begin + 1, dest_end, big_begin, big_end, dest_begin + 1, dest_end, false);
			additer(dest_begin + 1, dest_end, small_begin, small_end, dest_begin + 1, dest_end, false);
			// a_big * b_big << 2 + (a_big * b_small + a_small * b_big) << 1 + a_small * b_small -> dest

#if debugprint
			{
				cout << "dest: " << 2 * n << " ={";
				for (auto &&p = container::iterator{dest_begin}; p != dest_end; p++)
				{
					cout << hex << (int)*p << ' ';
				}
				cout << "}" << endl;
			}
#endif

			return;
		}
		// EVEN n

		// a = a_hi << n/2 + a_lo
		// b = b_hi << n/2 + b_lo
		// a : n
		// b : n
		// a_hi : n/2
		// a_lo : n/2
		// b_hi : n/2
		// b_lo : n/2

		// dest : 2n
		// buff : 4n

		// z2 = a_hi * b_hi : n
		// z1 = a_hi * b_lo + a_lo * b_hi = (a_hi + a_lo) * (b_hi + b_lo) - z2 - z0 : n + 1 (one extra for optional carry)
		// z0 = a_lo * b_lo : n

		// a * b = z2 << n + z1 << n/2 + z0

		auto big_mid = big_begin + n / 2;
		auto small_mid = small_begin + n / 2;

		auto dest_mid = dest_begin + n;
		auto buff_mid = buff_begin + 2 * n;

		// if b_hi_empty then z2 = 0

		// Calc z2
		karatsuba(big_mid, big_end, small_mid, small_end, dest_mid, dest_end, buff_begin, buff_mid, n / 2);
		// z2 = a_hi * b_hi -> dest[n..] : n

		// Calc z0
		karatsuba(big_begin, big_mid, small_begin, small_mid, dest_begin, dest_mid, buff_begin, buff_mid, n / 2);
		// z0 = a_lo * b_lo -> dest[..n] : n

		// z2 << n + z0 -> dest

		auto buff_s1 = buff_begin + n / 2;
		auto buff_q1 = buff_begin + n;
		auto buff_q3 = buff_mid + n;

		bool a_carry = additer(move(big_mid), move(big_end), move(big_begin), move(big_mid), buff_s1, buff_q1, false);
		// a_hi + a_lo -> buff[n/2..n] : n/2

		bool b_carry = additer(move(small_begin), move(small_mid), move(small_mid), move(small_end), buff_begin, buff_s1, false);
		// b_hi + b_lo -> buff[0..n/2] : n/2

		karatsuba(move(buff_s1), move(buff_q1), move(buff_begin), move(buff_s1), buff_q1, buff_mid, buff_mid, buff_end, n / 2);
		// (a_hi + a_lo):n/2 * (b_hi + b_lo):n/2 -> buff[n..2n] : n
		// Since (a_hi + a_lo) and (b_hi + b_lo) could have overflown we need to calculate the overflow of the product.

		// Add z0 and z2 together for efficiency
		if (additer(dest_begin, dest_mid, dest_mid, dest_end, buff_mid, buff_q3, false))
		{
			*buff_q3 = base{1};
		}
		else
		{
			*buff_q3 = base{0};
		}
		// z0 + z2 -> buff[2n..3n+1] : n + 1

		auto dest_q1 = dest_begin + n / 2;
		auto dest_q3 = dest_mid + n / 2;
		
		additer(dest_q1, dest_end, buff_q1, buff_mid, dest_q1, dest_end, false);

		if (a_carry && b_carry)
		{
			// Both (a_hi + a_lo) and (b_hi + b_lo) have overflown
			// This means that the product should be :
			// ((a_hi + a_lo):n/2 + 1 << n/2) * ((b_hi + b_lo):n/2 + 1 << n/2) =
			//		1 << n + ((a_hi + a_lo):n/2 + (b_hi + b_lo):n/2) << n/2 + (a_hi + a_lo):n/2 * (b_hi + b_lo):n/2
			// We add the (1 << n) here
			additer(dest_q3, dest_end, Nil.data.begin(), Nil.data.end(), dest_q3, dest_end, true);
			// We add the (<< n/2) term of this product here
			// add ((a_hi + a_lo) << n/2)
			additer(dest_q1 + n / 2, dest_end, buff_s1, buff_q1, dest_q1 + n / 2, dest_end, false);
			// add ((b_hi + b_lo) << n/2)
			additer(dest_q1 + n / 2, dest_end, buff_begin, buff_s1, dest_q1 + n / 2, dest_end, false);

#if debugprint
			{
				cout << "both carry" << endl;
			}
#endif

			// 1 << n + ((a_hi + a_lo):n/2 + (b_hi + b_lo):n/2) << n/2 + (a_hi + a_lo):n/2 * (b_hi + b_lo):n/2 = (a_hi + a_lo) * (b_hi + b_lo) -> {buff[n..2n], c} : n + 1
		}
		else
		{
			if (a_carry)
			{
				// Just (a_hi + a_lo) has overflown
				// This means that the product should be :
				// (a_hi + a_lo + 1 << n/2) * (b_hi + b_lo) = (b_hi + b_lo) << n/2 + (a_hi + a_lo) * (b_hi + b_lo)
				// We add the (<< n/2) term of this product here
				additer(dest_q1 + n / 2, dest_end, buff_begin, buff_s1, dest_q1 + n / 2, dest_end, false);
				// ((a_hi + a_lo):n/2 + 1 << n/2) * (b_hi + b_lo):n/2 = (b_hi + b_lo):n/2 << n/2 + (a_hi + a_lo):n/2 * (b_hi + b_lo):n/2

#if debugprint
				{
					cout << "a carry" << endl;
				}
#endif
			}
			if (b_carry)
			{
				// Same as above but other way around
				additer(dest_q1 + n / 2, dest_end, buff_s1, buff_q1, dest_q1 + n / 2, dest_end, false);

#if debugprint
				{
					cout << "b carry" << endl;
				}
#endif
			}
		}

		// Substract z0 and z2 from (a_hi * a_lo) * (b_hi * b_lo) to get z1
		subiter(dest_q1, dest_end, buff_mid, buff_q3 + 1, dest_q1, dest_end, nullptr, false);
		// z2 << n + z1 << n/2 + z0 -> dest : n

#if debugprint
		{

			cout << "dest: " << 2 * n << " ={";
			for (auto &&p = container::iterator{dest_begin}; p != dest_end; p++)
			{
				cout << hex << (int)*p << ' ';
			}
			cout << "}" << endl;
		}
#endif

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
	void dint::mult(container::iterator &&a_begin, container::iterator &&a_end, container::iterator &&b_begin, container::iterator &&b_end, container::iterator dest_begin, container::iterator dest_end, container::iterator &&buff_begin, container::iterator &&buff_end)
	{
		size_t n = a_end - a_begin;

		dint::karatsuba(move(a_begin), move(a_end), move(b_begin), move(b_end), dest_begin, dest_end, buff_begin, buff_end, n);
	}

	void dint::mult(const container &&a, const container &&b, container &dest)
	{
		size_t sa = a.size();
		size_t sb = b.size();

		container at{a};
		container bt{b};

		container buff;

		if (sa >= sb)
		{
			bt.resize(sa);
			buff.resize(sa * 4);

			mult(move(at.begin()), move(at.end()), move(bt.begin()), move(bt.end()), dest.begin(), dest.end(), move(buff.begin()), move(buff.end()));
		}
		else
		{
			at.resize(sb);
			buff.resize(sb * 4);

			mult(move(bt.begin()), move(bt.end()), move(at.begin()), move(at.end()), dest.begin(), dest.end(), move(buff.begin()), move(buff.end()));
		}
	}

	dint operator*(const dint &a, const dint &b)
	{
		dint res;

		size_t sa = a.data.size();
		size_t sb = b.data.size();

		container at{a.data};
		container bt{b.data};

		if (sa >= sb)
		{
			bt.resize(sa);
			res.data = container(2 * sa);
		}
		else
		{
			at.resize(sb);
			res.data = container(2 * sb);
		}

		container &dest = res.data;

		dint::mult(move(at), move(bt), dest);

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