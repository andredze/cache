#include "cache.hpp"

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class LRUCache : public Cache<PageT, KeyT> {
public:
    using Cache<PageT, KeyT>::size_;
    using Cache<PageT, KeyT>::cache_;
    using Cache<PageT, KeyT>::hash_map_;

	bool Add (KeyT key, PageT (*slow_get_page) (KeyT));
};

//--------------------------------------------------------------------------------