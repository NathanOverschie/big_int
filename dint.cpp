#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

namespace bigint
{

	typedef unsigned long long base;
	typedef std::vector<base> container;

	using namespace std;

	/**
	 * @brief A class for dynamic integers. This class is a integer value class for integer of an arbitrary size.
	 *
	 */
	class dint
	{

	private:
		// data represents the integer in words of size base
		// data[0] is the LSB
		// data has at least size 1
		// the most significant word is not 0 except if the total is 0
		container data{0};
		bool negative{false};

		void remove_leading_zeros();

		/// @brief adds the two dints together, or substracts b from a.
		/// @param a first addition argument
		/// @param b second addition argument
		/// @param dest the result will go into this dint
		/// @param subtract if false will to addition if true will do substraction
		/// @param c inital carry
		/// @invariant \code{(c => a > b)}
		void add(const dint &&a, const dint &&b, dint &dest, const bool substract, const bool increment);

	public:
		dint() = default;
		dint(const dint &) = default;
		dint(dint &&) = default;

		/**
		 * @brief Construct a new dint object
		 *
		 * @param arg an integer type that will be converted to dint
		 */
		dint(const base &arg)
		{
			data[0] = arg;
		}

		dint(const container &arg)
		{
			data = arg;
			remove_leading_zeros();
		}

		dint(container &&arg)
		{
			data = arg;
			remove_leading_zeros();
		}

		dint &operator=(const dint &) = default;
		dint &operator=(dint &&) = default;

		friend dint operator+(const dint &, const dint &);
		friend dint operator+(const dint &, dint &&);

		friend bool absgrt(const dint &, const dint &);
		friend bool abslst(const dint &, const dint &);
		friend bool operator>(const dint &, const dint &);
		friend bool operator<(const dint &, const dint &);
		friend bool operator==(const dint &, const dint &);

		bool operator!=(const dint &a) const
		{
			return !operator==(*this, a);
		}

		bool operator<=(const dint &a) const
		{
			return !operator>(*this, a);
		}

		bool operator>=(const dint &a) const
		{
			return !operator<(*this, a);
		}

		void operator+=(const dint &);

		void operator+=(base);

		void operator-=(const dint &a)
		{
			negative = !negative;
			operator+=(a);
			negative = !negative;
		}

		dint operator++(int)
		{
			dint tmp{*this};
			operator+=(dint{1});
			return tmp;
		}

		dint &operator++()
		{
			operator+=(dint{1});
			return *this;
		}

		dint operator--(int)
		{
			dint tmp{*this};
			operator+=(-dint{1});
			return tmp;
		}

		dint &operator--()
		{
			operator+=(-dint{1});
			return *this;
		}

		dint operator-() const
		{
			dint res{*this};
			res.negative = !res.negative;
			return res;
		}

		~dint() = default;

		string toHexString() const;

		size_t size() const
		{
			return data.size();
		}

		base front() const
		{
			return data.front();
		}

		base back() const
		{
			return data.back();
		}

		bool neg() const
		{
			return negative;
		}
	};

	static const dint empty{container{}};

	void dint::add(const dint &&big, const dint &&small, dint &dest, const bool substract = false, const bool increment = false)
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
			if (!substract)
			{
				t = *pbig;
				*pdest = t + *psmall + c;
				c = (*pdest >= t + c ? 0 : 1);
			}
			else
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
		}

		// Second part of the calculation, only one number contributes to the result
		for (; pbig != big.data.end() && (c == 1 || big != dest); pbig++, pdest++)
		{
			if (!substract)
			{
				*pdest = *pbig + c;
				c = (*pdest >= c ? 0 : 1);
			}
			else
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
		}

		if (!substract)
		{
			// Optionally add an extra word to store the leftover carry bit in.
			if (c == 1 && pbig == big.data.end())
			{
				dest.data.push_back(base{1});
			}
		}
		else
		{
			// Remove leading zeros
			if (zeros)
			{
				dest.data.erase(pzeros, dest.data.end());
			}
		}
	}

	dint operator+(const dint &a, const dint &b)
	{

		size_t sa = a.data.size();
		size_t sb = b.data.size();

		size_t max;
		size_t min;

		const container *dmax;
		const container *dmin;
		dint res;

		// Assign the biggest number to max and dmax, and the smallest to min and dmin
		if (a > b)
		{
			max = a.data.size();
			min = b.data.size();

			dmax = &a.data;
			dmin = &b.data;

			// The result will take the sign of the biggest number wether it is addition or subtraction
			res.negative = a.negative;
		}
		else
		{
			max = b.data.size();
			min = a.data.size();

			dmax = &b.data;
			dmin = &a.data;
			// The result will take the sign of the biggest number wether it is addition or subtraction
			res.negative = b.negative;
		}

		// The result vector has the size of the biggest number (possibly 1 extra word for the carry but thats done later)
		res.data = container(max);

		// Create a reference to the result data
		container &dr{res.data};

		if (a.negative == b.negative)
		{
			// Addition
			size_t i;
			char c = 0;

			// first part of addition: both numbers contribute to the result
			for (i = 0; i < min; i++)
			{
				dr[i] = (*dmax)[i] + (*dmin)[i] + c;
				c = dr[i] >= (*dmax)[i] + c ? 0 : 1;
			}

			// second part of addition: only biggest number contributes to the result
			for (; i < max && c == 1; i++)
			{
				dr[i] = (*dmax)[i] + c;
				c = dr[i] >= c ? 0 : 1;
			}

			// Optionally add carry word
			if (c == 1 && i == max)
			{
				dr.push_back(base{1});
			}
		}
		else
		{
			// Substraction
			size_t i;
			char c = 0;
			// first part of the subtraction: both numbers contribute to the result
			for (i = 0; i < min; i++)
			{
				dr[i] = (*dmax)[i] - (*dmin)[i] - c;
				c = dr[i] <= (*dmax)[i] ? 0 : 1;
			}
			// second part of the subtraction: only biggest number contributes to the result
			for (; i < max; i++)
			{
				dr[i] = (*dmax)[i] - c;
				c = dr[i] <= (*dmax)[i] ? 0 : 1;
			}

			// find leading words with zeros
			for (i = max; i > 1; i--)
			{
				if (dr[i] != 0)
					break;
			}

			// delete words with leading zeros
			dr.resize(i);
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

		for (int i = a.size() - 1; i >= 0; i--)
		{
			if (a.data[i] < b.data[i])
			{
				return true;
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

		for (int i = a.size() - 1; i >= 0; i--)
		{
			if (a.data[i] > b.data[i])
			{
				return true;
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
				add(move(a), move(*this), *this, true);
				negative = a.negative;
			}
			else
			{
				add(move(*this), move(a), *this, true);
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
	dint f{1};

	for (dint i = 1; i <= 1000000; i++)
	{
		f += i;
	}

	cout << f.toHexString() << endl;

	return 0;
}
