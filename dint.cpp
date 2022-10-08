#include "dint.h"

namespace bigint
{

	/**
	 * @brief A class for dynamic integers. This class is a integer value class for integer of an arbitrary size.
	 *
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

	dint operator+(const dint &a, dint &&b)
	{
		b.operator+=(a);
		return b;
	}

	bool abslst(const dint &a, const dint &b)
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

	bool absgrt(const dint &a, const dint &b)
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
			return abslst(a, b);
		}
		if (a.neg())
		{
			return false;
		}
		if (b.neg())
		{
			return true;
		}
		return absgrt(a, b);
	}

	bool operator<(const dint &a, const dint &b)
	{
		if (a.neg() && b.neg())
		{
			return absgrt(a, b);
		}
		if (a.neg())
		{
			return true;
		}
		if (b.neg())
		{
			return false;
		}
		return abslst(a, b);
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

	/**
	 * @brief Give a hex string representation of dint
	 *
	 * @return hex string representations
	 */
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

	dint mult(const dint &a, base b)
	{
		dint res;

		for (base i = 0; i < b; ++i)
		{
			res += a;
		}

		return res;
	}

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

}

using namespace bigint;

int main(int argc, char const *argv[])
{

	dint f{};

	dint range{10000000};

	dint offset{13333};

	for (dint i = 1; i <= range; ++i)
	{

		f += offset;
	}

	cout << range.toHexString() << endl;
	cout << offset.toHexString() << endl;

	cout << f.toHexString() << endl;

	return 0;
}
