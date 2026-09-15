#include "lru.hpp"

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool LRU<PageT, KeyT>::Add (KeyT key, PageT slow_get_page)
{
	bool hit = LookUpElement (key);

	// FIXME: путаем key и page
	if (hit) {
		cache_.remove     (value);
		cache_.push_front (value);

		return true;
	}
	else {
		cache_.pop_back   ();
		cache_.push_front (value);
	}

	return false;
}

//————————————————————————————————————————————————————————————————————————————————
