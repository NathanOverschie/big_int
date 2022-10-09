#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <queue>
#include <string>
#include <sstream>
#include <list>
#include <string>

namespace bigint
{
	using namespace std;

	typedef unsigned char base;
	typedef vector<base> container;

	class dint
	{
	public:
		dint() = default;
		dint(const dint &) = default;
		dint(dint &&) = default;
		dint(unsigned long long);
		dint(const container &);
		dint(container &&);

		~dint() = default;

		dint &operator=(const dint &) = default;
		dint &operator=(dint &&) = default;


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


		friend dint operator+(const dint &, const dint &);
		friend dint operator+(const dint &, dint &&);

		friend dint operator-(const dint &, const dint &);
		friend dint operator-(const dint &, dint &&);

		friend bool operator>(const dint &, const dint &);
		friend bool operator<(const dint &, const dint &);
		friend bool operator==(const dint &, const dint &);

		string toHexString() const;

		size_t size() const;

		base front() const;

		base back() const;

		bool neg() const;

	private:
		// data represents the integer in words of size base
		// data.front() is the Least Significant Word
		// data.back() is the Most Significant Word
		// data has at least size 1
		// the most significant word is not 0 except if the total is 0
		container data{0};
		bool negative{false};

		void remove_leading_zeros();

		static void mult(const dint &&a, const dint&&b, dint &dest);
		static void karatsuba(container::iterator, container::iterator, container::iterator, container::iterator, container::iterator, container::iterator, size_t);


		static void add(const container &&a, const container &&b, container &dest, const bool incr);
		static void sub(const container &&a, const container &&b, container &dest, const bool decr);

		static bool absgrt(const dint &, const dint &);
		static bool abslst(const dint &, const dint &);

	};

	extern const dint& Nil;
}