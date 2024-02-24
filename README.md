# big_int
this is a library I made after taking a course on algebra at the TUe.
Its purpose is to provide a class for dynamically sized integers.
This is a core feature in other languages but is not present in the C++ standard library.

The way it is implemented is highly optimized for efficiency.
For example, the multiplication of two integers of each n-digit is done
with `O(n^(log_2(3))`, with the [Karatsuba algorithm](https://en.wikipedia.org/wiki/Karatsuba_algorithm)
