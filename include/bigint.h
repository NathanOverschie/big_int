#include "common.h"

namespace bigint
{
class bigint
{
  public:
	template <class const_iterator, class iterator>
		requires std::forward_iterator<const_iterator> && std::forward_iterator<iterator>
	static bool additer(const const_iterator &, const const_iterator &, const const_iterator &, const const_iterator &,
						const iterator &, const iterator &, const bool = false);

	template <class const_iterator, class iterator>
	static bool subiter(const const_iterator &, const const_iterator &, const const_iterator &, const const_iterator &,
						const iterator &, const iterator &, iterator *, const bool = false);
};
} // namespace bigint