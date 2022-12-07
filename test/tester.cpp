#include <dint.h>

#include <random>
#include <algorithm>

using namespace bigint;

bool testAddition(std::mt19937 gen, size_t n)
{
	std::uniform_int_distribution<unsigned long long> distrib(0, (numeric_limits<unsigned long long>::max() / 2) - 1);

	unsigned long long a, b, s;

	dint da, db, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distrib(gen);
		b = distrib(gen);
		s = a + b;

		// cout << a << '+' << b << '=' << s << ' ' << endl;

		da = dint(a);
		db = dint(b);
		ds = da + db;

		if (dint{s} != ds)
		{
			cout << endl;
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "s: " << hex << s << endl;
			cout << "ds: " << ds.toHexString() << endl;
			cout << "should have been:" << dint{s}.toHexString() << endl;

			return false;
		}

		// cout << "✓" << endl;
	}

	return true;
}

bool testSubstraction(std::mt19937 gen, size_t n)
{
	std::uniform_int_distribution<long long> distrib(numeric_limits<long long>::min(), numeric_limits<long long>::max());

	long long a, b, s;

	dint da, db, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distrib(gen);
		b = distrib(gen);
		s = a - b;

		da = dint(a);
		db = dint(b);
		ds = da - db;

		// cout << a << '-' << b << '=' << s << endl;

		if (dint{s} != ds)
		{
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "s: " << hex << s << endl;
			cout << "ds: " << ds.toHexString() << endl;
			cout << "should have been:" << dint{s}.toHexString() << endl;

			return false;
		}
	}

	return true;
}

bool testMultiplication(std::mt19937 gen, size_t n)
{
	std::uniform_int_distribution<unsigned long long> distrib(0, numeric_limits<unsigned long long>::max() >> (sizeof(unsigned long long) * __CHAR_BIT__ / 2));

	unsigned long long a, b, s;

	dint da, db, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distrib(gen);
		b = distrib(gen);
		s = a * b;

		da = dint(a);
		db = dint(b);
		ds = da * db;

		// cout << a << '*' << b << '=' << s << endl;

		if (dint{s} != ds)
		{
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "s: " << hex << s << endl;
			cout << "ds: " << ds.toHexString() << endl;
			cout << "should have been:" << dint{s}.toHexString() << endl;

			return false;
		}
	}

	return true;
}

bool testMultiplicationWithBase(std::mt19937 gen, size_t n)
{
	std::uniform_int_distribution<unsigned long long> distriba(0, numeric_limits<unsigned long long>::max() >> ((sizeof(base) + 1) * __CHAR_BIT__ / 2));
	std::uniform_int_distribution<base> distribb(0, numeric_limits<base>::max());

	unsigned long long a, s;
	base b;

	dint da, db, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distriba(gen);
		b = distribb(gen);
		s = a * b;

		da = dint(a);
		db = dint(static_cast<unsigned long long>(b));
		ds = da * db;

		// cout << a << '*' << static_cast<int>(b) << '=' << s << endl;

		if (dint{s} != ds)
		{
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << static_cast<int>(b) << endl;

			cout << "s: " << hex << s << endl;
			cout << "ds: " << ds.toHexString() << endl;
			cout << "should have been:" << dint{s}.toHexString() << endl;

			return false;
		}
	}

	return true;
}

namespace {
		inline unsigned long long hi_n(unsigned long long x, int n)
		{
			return (x >> (n / 2)) & (1ULL << (n) - 1);
		}

		inline unsigned long long lo_n(unsigned long long x, int n)
		{
			return ((1ULL << (n / 2)) - 1) & x;
		}

		// unsigned long long printzs(unsigned long long a, unsigned long long b, int bits){
		// 	cout << hex << a << endl;
		// 	cout << hex << b << endl;

		// 	if(bits == __CHAR_BIT__){
		// 		return;
		// 	}

		// 	unsigned long long z2, z1, z0, ret;

		// 	z2 = printzs(hi_n(a, bits), hi_n(b, bits), bits/2);
		// 	z0 = printzs(lo_n(a, bits), lo_n(b, bits), bits/2);

		// 	z1 = printzs(hi_n(a, bits) + lo_n(a, bits), hi_n(b, bits) + lo_n(b, bits), bits / 2) - z2 - z0;

		// 	ret = z2 << bits + z1 << bits/2 + z0;
		// }
}

bool testMultiplicationSimple(std::mt19937 gen, size_t n)
{
	std::uniform_int_distribution<unsigned long long> distriba(1UL << (sizeof(base) * __CHAR_BIT__ * 3), 1UL << (sizeof(base) * __CHAR_BIT__ * 4));
	std::uniform_int_distribution<unsigned long long> distribb(0, 1UL << (sizeof(base) * __CHAR_BIT__ * 2));

	unsigned long long a, b, s;

	dint da, db, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distribb(gen);
		b = distribb(gen);
		s = a * b;

		da = dint(a);
		db = dint(b);
		ds = da * db;

		cout << a << '*' << b << '=' << s << endl;

		if (dint{s} != ds)
		{
			cout << "error" << endl;
			cout << "n = " << dec << i << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "ds: \t" << ds.toHexString() << endl;
			cout << "s: \t" << dint{s}.toHexString() << endl;


			return false;
		}
	}

	return true;
}

int main(int argc, char const *argv[])
{
	std::random_device rd;	// Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

	size_t n = 100000;

	bool MultSimple = testMultiplicationSimple(gen, n);
	// bool Add = testAddition(gen, n);
	// bool Sub = testSubstraction(gen, n);
	// bool Mult = testMultiplication(gen, n);
	// bool MultWithBase = testMultiplicationWithBase(gen, n);

	// if (Add)
	// 	cout << "Addition good" << endl;

	// if (Sub)
	// 	cout << "Substraction good" << endl;

	// if (Mult)
	// 	cout << "Multiplication good" << endl;

	// if (MultWithBase)
	// 	cout << "Multiplication with base good" << endl;

	if (MultSimple)
		cout << "Multiplication with numbers of size of a power of 2 good" << endl;

	return 0;
}
