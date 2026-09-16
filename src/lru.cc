#include "lru.hpp"
#include <iostream>

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool LRUCache<PageT, KeyT>::Add (KeyT key)
{
	PageT* page_p = LookUpElement (key);
	
	// FIXME: путаем key и page
	if (page_p) {
		cache_.remove     (page_p);
		cache_.push_front (page_p);

		return true;
	}
	else {
		cache_.pop_back   ();
		cache_.push_front (page_p);
	}

	return false;
}

//————————————————————————————————————————————————————————————————————————————————
