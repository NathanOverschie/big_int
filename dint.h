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

	private:
		// data represents the integer in words of size base
		// data.front() is the Least Significant Word
		// data.back() is the Most Significant Word
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
		static void add(const dint &&a, const dint &&b, dint &dest, const bool incr);
		static void sub(const dint &&a, const dint &&b, dint &dest, const bool decr);

	public:
		dint() = default;
		dint(const dint &) = default;
		dint(dint &&) = default;

		dint(unsigned long long arg);

		dint(const container &arg);

		dint(container &&arg);

		dint &operator=(const dint &) = default;
		dint &operator=(dint &&) = default;

		friend dint operator+(const dint &, const dint &);
		friend dint operator+(const dint &, dint &&);

		friend bool absgrt(const dint &, const dint &);
		friend bool abslst(const dint &, const dint &);
		friend bool operator>(const dint &, const dint &);
		friend bool operator<(const dint &, const dint &);
		friend bool operator==(const dint &, const dint &);

		bool operator!=(const dint &a) const;
		bool operator<=(const dint &a) const;
		bool operator>=(const dint &a) const;

		void operator+=(const dint &);

		void operator+=(base);

		void operator-=(const dint &a);

		dint operator++(int);

		dint &operator++();

		dint operator--(int);

		dint &operator--();

		dint operator-() const;

		~dint() = default;

		string toHexString() const;

		size_t size() const;

		base front() const;

		base back() const;

		bool neg() const;
	};

	extern const dint& Nil;
}