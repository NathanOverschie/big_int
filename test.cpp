#include "dint.h"

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

	dint f{};

	dint range{10000000};

	dint offset{13333};

	for (dint i = 1; i <= range; ++i)
	{

		f += offset;
	}

	cout << range.toHexString() << endl;
	cout << offset.toHexString() << endl;

	cout << f.toHexString() << endl;

	return 0;
}
