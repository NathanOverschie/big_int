#include "common.h"
#include "bigint.h"

namespace bigint
{
using namespace std;

using base		= unsigned char;
using container = vector<base>;

using iterator		 = container::iterator;
using const_iterator = container::const_iterator;

constexpr unsigned short bits_per_word = sizeof(base) * __CHAR_BIT__;

class dint : private bigint
{
  public:
	dint()			   = default;
	dint(const dint &) = default;
	dint(dint &&)	   = default;
	dint(unsigned long long);
	explicit dint(long long);
	dint(const container &);
	dint(container &&);

	~dint() = default;

	dint &operator=(const dint &) = default;
	dint &operator=(dint &&)	  = default;

	bool operator!=(const dint &) const;
	bool operator<=(const dint &) const;
	bool operator>=(const dint &) const;

	void operator+=(const dint &);
	void operator+=(base);

	void operator-=(const dint &);
	void operator-=(base);

	dint operator++(int);
	dint &operator++();

	dint operator--(int);
	dint &operator--();

	dint operator-() const;

	dint operator<<(unsigned int) const;
	dint operator>>(unsigned int) const;

	dint &operator<<=(unsigned int);
	dint &operator>>=(unsigned int);

	friend dint operator+(const dint &, const dint &);
	friend dint &operator+(const dint &, dint &&);

	friend dint operator-(const dint &, const dint &);
	friend dint &operator-(const dint &, dint &&);

	friend bool operator>(const dint &, const dint &);
	friend bool operator<(const dint &, const dint &);
	friend bool operator==(const dint &, const dint &);

	friend dint operator*(const dint &, const dint &);
	friend dint operator*(const dint &, base);

	friend void mult(const dint &, const dint &, dint &);

	void operator*=(const dint &);
	void operator*=(base);

	string toHexString() const;

	size_t size() const;

	base front() const;

	base back() const;

	bool neg() const;

	void random(int size, std::uniform_int_distribution<base> distr, std::mt19937 gen);

  private:
	// data represents the integer in words of size base
	// data.front() is the Least Significant Word
	// data.back() is the Most Significant Word
	// data has at least size 1
	// the most significant word is not 0 except if the total is 0
	container data{0};
	bool negative{false};

	void remove_leading_zeros();

	void shiftwordsright(size_t);
	void shiftwordsleft(size_t);

	static void karatsuba(const const_iterator &, const const_iterator &, const const_iterator &,
						  const const_iterator &, iterator, iterator, const iterator &, const iterator &, size_t);

	static void add(const container &a, const container &b, container &dest, const bool incr);

	static void sub(const container &a, const container &b, container &dest, const bool incr);

	static bool absgrt(const dint &, const dint &);
	static bool abslst(const dint &, const dint &);
};

extern const dint &Nil;

} // namespace bigint