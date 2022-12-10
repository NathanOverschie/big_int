#include "dint.h"

namespace bigint
{
	const dint &Nil{container{{}}};

	dint::dint(unsigned long long arg) : data{}
	{
		int words = sizeof(unsigned long long) / sizeof(base);

		for (size_t i = 0; i < words && arg != 0ULL; i++)
		{
			data.push_back(static_cast<base>(arg));
			arg >>= (8 * sizeof(base));
		}

		remove_leading_zeros();
	}

	dint::dint(long long arg) : data{}
	{
		negative = (arg < 0);
		dint(static_cast<unsigned long long>(arg));
	}

	dint::dint(const container &arg) : data{arg}
	{
		remove_leading_zeros();
	}

	dint::dint(container &&arg) : data{arg}
	{
		remove_leading_zeros();
	}

	bool dint::operator!=(const dint &a) const
	{
		return !operator==(*this, a);
	}

	bool dint::operator<=(const dint &a) const
	{
		return !operator>(*this, a);
	}

	bool dint::operator>=(const dint &a) const
	{
		return !operator<(*this, a);
	}

	dint dint::operator-() const
	{
		dint res{*this};
		res.negative = !res.negative;
		return res;
	}

	size_t dint::size() const
	{
		return data.size();
	}

	base dint::front() const
	{
		return data.front();
	}

	base dint::back() const
	{
		return data.back();
	}

	bool dint::neg() const
	{
		return negative;
	}

	bool dint::abslst(const dint &a, const dint &b)
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

	bool dint::absgrt(const dint &a, const dint &b)
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
			return dint::abslst(a, b);
		}
		if (a.neg())
		{
			return false;
		}
		if (b.neg())
		{
			return true;
		}
		return dint::absgrt(a, b);
	}

	bool operator<(const dint &a, const dint &b)
	{
		if (a.neg() && b.neg())
		{
			return dint::absgrt(a, b);
		}
		if (a.neg())
		{
			return true;
		}
		if (b.neg())
		{
			return false;
		}
		return dint::abslst(a, b);
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

	dint dint::operator<<(unsigned int n) const
	{
		dint res{*this};

		res.operator<<=(n);

		return res;
	}

	dint dint::operator>>(unsigned int n) const
	{
		dint res{*this};

		res.operator<<=(n);

		return res;
	}

	dint &dint::operator<<=(unsigned int n)
	{
		unsigned int m = n % bits_per_word;

		base t = 0;

		for (auto &&i = data.begin(); i != data.end(); i++)
		{
			base x = *i;
			*i = (*i << m) | t;

			t = (x & (numeric_limits<base>::max() << (bits_per_word - m))) >> (bits_per_word - m);
		}

		if (t > 0)
		{
			data.push_back(t);
		}

		shiftwordsleft(n / bits_per_word);

		return *this;
	}

	dint &dint::operator>>=(unsigned int n)
	{
		unsigned int m = n % bits_per_word;

		base t = 0;
		base x;

		for (auto &&i = data.rbegin(); i != data.rend(); i++)
		{
			x = *i;
			*i = (*i >> m) | t;
			t = (x & (numeric_limits<base>::max() >> (bits_per_word - m))) << (bits_per_word - m);
		}

		if (data.back() == 0)
		{
			data.pop_back();
		}

		shiftwordsright(n / bits_per_word);

		return *this;
	}

	void dint::shiftwordsright(size_t n)
	{
		if (n >= size())
		{
			data = container{{0}};
		}
		else
		{
			data.erase(data.begin(), data.begin() + n);
		}
	}

	void dint::shiftwordsleft(size_t n)
	{
		container t(n, base{0});
		t.insert(t.end(), data.begin(), data.end());

		data = t;
	}

	void dint::remove_leading_zeros()
	{

		auto q = data.rbegin();
		for (; q != data.rend(); q++)
		{
			if (*q != 0)
			{
				break;
			}
		}

		if (q != data.rend())
		{
			data.erase(q.base(), data.end());
		}
		else
		{
			data.resize(1);
		}
	}

	void dint::random(int size, std::uniform_int_distribution<base> distr, std::mt19937 gen)
	{
		data.resize(size);

		for (auto &i : data)
		{
			i = distr(gen);
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

		vector<base> v{data};

		for (size_t i = v.size(); i != 0; i--)
		{
			r << hex << setw(sizeof(base) * 2) << setfill('0');
			r << static_cast<unsigned long long>(v[i - 1]);
			r << ' ';
		}

		return r.str();
	}

}
