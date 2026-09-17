#include <cstdlib>
#include "lru.hpp"
#include "error_handle.hpp"

template <typename PageT, typename KeyT>
PageT slow_get_page (KeyT key)
{
	for (size_t i = 0; i < 10000000; i++) {
		int tmp = 0;
	}

	return (PageT) key;
}

//————————————————————————————————————————————————————————————————————————————————

int main ()
{
	// std::deque<int> data = ReadIntDataFromCin ();

	//Cache<int, int> cache (3);

	LRUCache<int, int> lru_cache (3);

	for (int i = 0; i < 10; i++) {
		lru_cache.Add (i, slow_get_page);
	}

	lru_cache.Add (8, slow_get_page);
	//TODO: Dump has not tested because i not sure about your input foo
	lru_cache.Dump ("test1.txt");

	return EXIT_FAILURE;
}

//————————————————————————————————————————————————————————————————————————————————
