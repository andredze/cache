#include "caches/cache.hpp"

//————————————————————————————————————————————————————————————————————————————————

std::string GetSlowPage (const int& key);

std::string GetSlowPage (const int& key)
{
    return "page_" + std::to_string (key);
}

//==================================================

int main ()
{
    caches::MultiLvlCache<std::string, int> multi_cache {
        {{caches::Policy::TwoQ, 10},
         {caches::Policy::LRU,  15},
         {caches::Policy::LIRS, 17, 3}},
        GetSlowPage
    };

    return EXIT_SUCCESS;
}

//————————————————————————————————————————————————————————————————————————————————
