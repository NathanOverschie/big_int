#include <dint.h>

#include <random>
#include <chrono>
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

			cout << "ds:\t" << ds.toHexString() << endl;
			cout << "s:\t" << dint{s}.toHexString() << endl;

			throw runtime_error("");
		}
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

		if (dint{s} != ds)
		{
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "ds:\t" << ds.toHexString() << endl;
			cout << "s:\t" << dint{s}.toHexString() << endl;

			throw runtime_error("");
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

		if (dint{s} != ds)
		{
			cout << "error" << endl;
			cout << "n = " << i << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "ds:\t" << ds.toHexString() << endl;
			cout << "s:\t" << dint{s}.toHexString() << endl;

			throw runtime_error("");
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

	dint da, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distriba(gen);
		b = distribb(gen);
		s = a * b;

		da = dint(a);
		ds = da * b;

		if (dint{s} != ds)
		{
			cout << "error" << endl;
			cout << "n = " << i << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << static_cast<int>(b) << endl;

			cout << "ds:\t" << ds.toHexString() << endl;
			cout << "s:\t" << dint{s}.toHexString() << endl;

			throw runtime_error("");
		}
	}

	return true;
}

bool testMultiplicationSimple(std::mt19937 gen, size_t n, int size)
{
	std::uniform_int_distribution<unsigned long long> distrib(0, (1UL << (sizeof(base) * __CHAR_BIT__ * size)) - 1);

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

		if (dint{s} != ds)
		{
			cout << "error" << endl;
			cout << "n = " << dec << i << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "ds: \t" << ds.toHexString() << endl;
			cout << "s: \t" << dint{s}.toHexString() << endl;

			throw runtime_error("");
		}
	}

	return true;
}

int main(int argc, char const *argv[])
{
	std::random_device rd;	// Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

	size_t n = 200;

	cout << testMultiplicationSimple(gen, n, 1);
	cout << testMultiplicationSimple(gen, n, 2);
	cout << testMultiplicationSimple(gen, n, 3);
	cout << testMultiplicationSimple(gen, n, 4);

	cout << testAddition(gen, n);
	cout << testSubstraction(gen, n);
	cout << testMultiplication(gen, n);
	cout << testMultiplicationWithBase(gen, n);

	return 0;
}
