#include "dint.h"

#include <random>
#include <algorithm>

using namespace bigint;

bool testAddition(std::mt19937 gen, size_t n)
{
	std::uniform_int_distribution<unsigned long long> distrib(0, numeric_limits<unsigned long long>::max() / 2 - 1);

	unsigned long long a, b, s;

	dint da, db, ds;

	for (size_t i = 0; i < n; i++)
	{
		a = distrib(gen);
		a = distrib(gen);
		s = a + b;

		da = dint(a);
		db = dint(b);
		ds = da + db;

		if (dint{s} != ds)
		{
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "s: " << hex << s << endl;
			cout << "ds: " << ds.toHexString() << endl;

			return false;
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
		a = distrib(gen);
		s = a + b;

		da = dint(a);
		db = dint(b);
		ds = da + db;

		if (dint{s} != ds)
		{
			cout << "error" << endl;

			cout << "a: " << hex << a << endl;
			cout << "b: " << hex << b << endl;

			cout << "s: " << hex << s << endl;
			cout << "ds: " << ds.toHexString() << endl;

			return false;
		}
	}

	return true;
}


int main(int argc, char const *argv[])
{
	std::random_device rd;	// Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	
	size_t n =  1000000;

	if(testAddition(gen, n))
		cout << "Addition good" << endl;

	if(testSubstraction(gen, n))
		cout << "Substraction good" << endl;

	return 0;
}
