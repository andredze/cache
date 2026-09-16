#include "cache.hpp"

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::ContainsKey (KeyT key)
{
    return hash_map_.contains (key);
}

//--------------------------------------------------------------------------------