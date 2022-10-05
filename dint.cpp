#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

namespace bigint
{

	typedef unsigned char base;

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
		vector<base> data{0};
		bool negative{false};

		void remove_leading_zeros();

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

		dint(const vector<base> &arg)
		{
			data = arg;
			remove_leading_zeros();
		}

		dint(vector<base> &&arg)
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

	/**
	 * @brief Add to numbers together
	 *
	 * @param a
	 * @param b
	 * @return result
	 */
	dint operator+(const dint &a, const dint &b)
	{

		size_t sa = a.data.size();
		size_t sb = b.data.size();

		size_t max;
		size_t min;

		const vector<base> *dmax;
		const vector<base> *dmin;
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
		res.data = vector<base>(max);

		// Create a reference to the result data
		vector<base> &dr{res.data};

		if (a.negative == b.negative)
		{
			// Addition
			size_t i;
			char c = 0;

			// first part of addition: both numbers contribute to the result
			for (i = 0; i < min; i++)
			{
				dr[i] = (*dmax)[i] + (*dmin)[i] + c;
				c = dr[i] >= (*dmax)[i] ? 0 : 1;
			}

			// second part of addition: only biggest number contributes to the result
			for (; i < max; i++)
			{
				dr[i] = (*dmax)[i] + c;
				c = dr[i] >= (*dmax)[i] ? 0 : 1;
			}

			// Optionally add carry word
			if (c == 1)
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

	void dint::operator+=(const dint &a)
	{
		// save which one is bigger for later
		bool abigger{absgrt(a, *this)};

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
			char c = 0;
			base t;
			size_t i;
			for (i = 0; i < sa; i++)
			{
				data[i] += a.data[i] + c;
				// check if there was overflow
				c = data[i] >= a.data[i] ? 0 : 1;
			}

			for (; i < sb; i++)
			{
				data[i] += c;
				c = data[i] >= c ? 0 : 1;
			}

			// append optional carry word
			if (c == 1)
			{
				data.push_back(base{1});
			}
		}
		else
		{
			// Substraction
			if (abigger)
			{
				// a - *this
				char c = 0;
				for (size_t i = 0; i < sa; i++)
				{
					data[i] = a.data[i] - data[i] - c;
					// check if there was 'under' flow
					c = data[i] <= a.data[i] ? 0 : 1;
				}

				// result takes the sign of the biggest number
				negative = a.negative;

				remove_leading_zeros();
			}
			else
			{
				// *this - a
				char c = 0;
				for (size_t i = 0; i < sa; i++)
				{
					data[i] = data[i] - a.data[i] - c;
					// check if there was 'under' flow
					c = data[i] <= a.data[i] ? 0 : 1;
				}

				for (size_t i = 0; i < sb; i++)
				{
					data[i] -= c;
					c = data[i] <= c ? 0 : 1;
				}

				// result keeps the sign of the biggest number
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

	dint mult(const dint &a, const dint &b)
	{
		dint res;

		for (dint i = 0; i < b; ++i)
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
	dint t {{0x00, 0x58, 0x77, 0x77, 0x30, 0x01}};

	cout << t.toHexString() << endl;

	t = mult(t, 0x10);

	cout << t.toHexString() << endl;

	return 0;
}
