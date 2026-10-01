#pragma once

//--------------------------------------------------------------------------------

#include "cache.hpp"

//--------------------------------------------------------------------------------

namespace lfu {

template <typename PageT, typename KeyT>
class LFU {

struct Node {
    KeyT   key_;
    size_t freq_;
    PageT  value_;

    typename std::list<Node*>::iterator it_;
};

    unordered_map<KeyT  , Node*>  key_map;
    unordered_map<size_t, std::list<Node*>> freq_map;

    int min_freq;
};

};