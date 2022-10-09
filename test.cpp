#include "dint.h"

#include <random>

using namespace bigint;

dint mult(const dint &a, base b)
{
	dint res;

	for (base i = 0; i < b; ++i)
	{
		res += a;
	}

	return res;
}

int main(int argc, char const *argv[])
{
	std::random_device rd;	// Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<unsigned long long> distrib(0, numeric_limits<unsigned long long>::max() / 2 - 1);

	unsigned long long a, b, s;

	dint da, db, ds;

	for (size_t i = 0; i < 10000000; i++)
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

			return 0;
		}
	}

	cout << "no errors" << endl;

	return 0;
}
