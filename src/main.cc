#include <cstdlib>
#include "lru.hpp"
#include "error_handle.hpp"

template <typename PageT, typename KeyT>
PageT slow_get_page (KeyT key)
{
	for (std::size_t i = 0; i < 10000000; i++) {
		int tmp = 0;
	}

	return (PageT) key;
}

//————————————————————————————————————————————————————————————————————————————————

int main ()
{
	LRUCache<int, int> lru_cache (3);

	return EXIT_FAILURE;
}

//————————————————————————————————————————————————————————————————————————————————
