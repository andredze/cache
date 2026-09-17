#include "lru.hpp"
#include <list>

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool LRUCache<PageT, KeyT>::Add (KeyT key, PageT (*slow_get_page) (KeyT))
{
	bool hit = LookUpElement (key);

	if (hit) {
		auto node_it = hash_map_.at (key);

	    cache_.splice (cache_.begin (), cache_, node_it);

		return true;
	}
	else {
        PageT page = slow_get_page (key); 

        if (cache_.size () == size_) {
            auto prev_key = cache_.back ();

            hash_map_.erase (prev_key);
            
            cache_.pop_back ();
        }


		cache_.push_front (page);
        hash_map_[key] = cache_.begin ();

	}

	return false;
}

//————————————————————————————————————————————————————————————————————————————————
