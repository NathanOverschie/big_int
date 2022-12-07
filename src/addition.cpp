#include "dint.h"

namespace bigint
{

	bool dint::additer(
		container::const_iterator &&big_begin,
		container::const_iterator &&big_end,
		container::const_iterator &&small_begin,
		container::const_iterator &&small_end,
		container::iterator dest_begin,
		container::iterator dest_end,
		const bool increment = false)
	{

		// Initialize the iterators
		auto pbig = container::const_iterator{big_begin};
		auto psmall = container::const_iterator{small_begin};
		auto pdest = container::iterator{dest_begin};

		base t;
		// Initialize the carry bit (can be one initially)
		base c = increment ? 1 : 0;

		// First part of calculation, both numbers contribute to the result
		for (; psmall != small_end; pbig++, psmall++, pdest++)
		{

			t = *pbig;
			*pdest = t + *psmall + c;
			c = (*pdest >= t + c ? 0 : 1);
		}

		bool self{big_begin == dest_begin};

		// Second part of the calculation, only one number contributes to the result
		for (; pbig != big_end && (c == 1 || !self); pbig++, pdest++)
		{

			*pdest = *pbig + c;
			c = (*pdest >= c ? 0 : 1);
		}

		// Return weither or not there was overflow.
		return c == 1 && pbig == big_end;
	}

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
	void dint::add(const container &&big, const container &&small, container &dest, const bool increment = false)
	{
		if (additer(big.cbegin(), big.cend(), small.cbegin(), small.cend(), dest.begin(), dest.end(), increment))
		{
			dest.push_back(base{1});
		}
	}

	bool dint::subiter(
		container::const_iterator &&big_begin,
		container::const_iterator &&big_end,
		container::const_iterator &&small_begin,
		container::const_iterator &&small_end,
		container::iterator dest_begin,
		container::iterator dest_end,
		container::iterator *pzeros,
		const bool increment = false)
	{
		// Initialize the iterators
		auto pbig = container::const_iterator{big_begin};
		auto psmall = container::const_iterator{small_begin};
		auto pdest = container::iterator{dest_begin};

		bool zeros = false;

		const bool check_zero(pzeros != nullptr);

		base t;
		// Initialize the carry bit (can be one initially)
		base c = increment ? 1 : 0;

		// First part of calculation, both numbers contribute to the result
		for (; psmall != small_end; pbig++, psmall++, pdest++)
		{
			t = *pbig;
			*pdest = *pbig - *psmall - c;

			c = (*pdest <= t - c ? 0 : 1);

			if (check_zero)
			{
				if (*pdest == 0)
				{
					if (!zeros)
					{
						*pzeros = pdest;
						zeros = true;
					}
				}
				else
				{
					zeros = false;
				}
			}
		}

		bool self{big_begin == dest_begin};

		// Second part of the calculation, only one number contributes to the result
		for (; pbig != big_end && (c == 1 || !self); pbig++, pdest++)
		{
			t = *pdest;
			*pdest -= c;
			c = ( t == 0 ? 1 : 0);

			if (check_zero)
			{
				if (*pdest == 0)
				{
					if (!zeros)
					{
						*pzeros = pdest;
						zeros = true;
					}
				}
				else
				{
					zeros = false;
				}
			}
		}

		// Return weither or not there was underflow.
		return c == 1 && pbig == big_end;
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
	void dint::sub(const container &&big, const container &&small, container &dest, const bool increment = false)
	{
		container::iterator *pzero = new container::iterator{};

		subiter(big.cbegin(), big.cend(), small.cbegin(), small.cend(), dest.begin(), dest.end(), pzero, increment);

		dest.erase(*pzero, dest.end());
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
			sub(move(this->data), move(Nil.data), this->data, true);
		}
		else
		{
			add(move(this->data), move(Nil.data), this->data, true);
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
			sub(move(this->data), move(Nil.data), this->data, true);
		}
		else
		{
			add(move(this->data), move(Nil.data), this->data, true);
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
			if (sa >= sb)
			{
				dint::add(move(a.data), move(b.data), res.data);
			}
			else
			{
				dint::add(move(b.data), move(a.data), res.data);
			}
		}
		else
		{
			// Substraction
			if (a > b)
			{
				dint::sub(move(a.data), move(b.data), res.data);
				res.negative = a.negative;
			}
			else
			{
				dint::sub(move(b.data), move(a.data), res.data);
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
	dint &operator+(const dint &a, dint &&b)
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
	dint &operator-(const dint &a, dint &&b)
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
			add(move(this->data), move(a.data), this->data);
		}
		else
		{
			// Substraction
			if (absgrt(a, *this))
			{
				sub(move(a.data), move(this->data), this->data);
				negative = a.negative;
			}
			else
			{
				sub(move(this->data), move(a.data), this->data);
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