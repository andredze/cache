#include "cache.hpp"

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::ContainsKey (KeyT key)
{
    return hash_map_.contains (key);
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
KeyT Cache<PageT, KeyT>::GenerateKey (PageT page)
{
    return (KeyT) page;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
void Cache<PageT, KeyT>::Dump (std::string file_name)
{
    std::ofstream file (file_name);
    
    if (!file) {
        return;
    }

    std::size_t i = 0;
    for (auto it = cache_.begin (); it != cache_.end (); it++, i++) {
        file << "elem " << i << " = " << *it << "\n";
    }
}

//————————————————————————————————————————————————————————————————————————————————