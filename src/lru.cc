#include <cache.hpp>

//————————————————————————————————————————————————————————————————————————————————

int lru (std::deque<int>* data, Cache* c)
{
    if (data->size () <= 0) {
        return 0;
    }

    int cur_value = GetNextCacheElement (data);

    
}