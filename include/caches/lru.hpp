
#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <list>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "input.hpp"

//--------------------------------------------------------------------------------

namespace caches {

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class LRU {

private:

    //==================================================

    using Node = std::pair<KeyT, PageT>;

    //==================================================

    std::list<Node> cache_;

    std::unordered_map<KeyT, typename std::list<Node>::iterator> hash_map_;

    //==================================================

    size_t capacity_;

    //==================================================

public:

    //==================================================

    LRU (std::size_t capacity) :
    capacity_ (capacity)
    {
        if (capacity == 0) {
            throw std::invalid_argument ("Capacity must be positive");
        }
    }

    //==================================================

    bool  Contain (const KeyT& key);
    bool  Hit     (const KeyT& key);
    PageT GetPage (const KeyT& key);

    std::optional<Node> Insert (const KeyT& key);
    std::optional<Node> Insert (const KeyT& key, PageT page);

    std::optional<PageT> Extract(const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LRU<PageT, KeyT>::Contain (const KeyT& key)
{
    return hash_map_.contain (key);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LRU<PageT, KeyT>::Hit (const KeyT& key)
{
    auto it = hash_map_.find (key);

    if (it != hash_map_.end ()) {
        cache_.splice (cache_.begin (), cache_, it->second);

        return true;
    }

    PageT page = GetSlowPage<PageT, KeyT> (key);

    if (cache_.size () == capacity_) {
        const KeyT& victim_key = cache_.back ().first;

        hash_map_.erase (victim_key);
        cache_.pop_back ();
    }

    cache_.emplace_front (key, std::move (page));

    hash_map_[key] = cache_.begin ();

    return false;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
PageT LRU<PageT, KeyT>::GetPage (const KeyT& key)
{
    Hit (key);

    return hash_map_.at (key)->second;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LRU<PageT, KeyT>::Node>
LRU<PageT, KeyT>::Insert (const KeyT& key)
{
    if (Contain (key)) {
        Hit (key);
        return std::nullopt;
    }

    PageT page = GetSlowPage<PageT, KeyT> (key);

    return Insert (key, std::move (page));
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LRU<PageT, KeyT>::Node>
LRU<PageT, KeyT>::Insert (const KeyT& key, PageT page)
{
    auto it = hash_map_.find (key);

    if (it != hash_map_.end ()) {
        cache_.splice (cache_.begin (), cache_, it->second);
        return std::nullopt;
    }

    std::optional<Node> victim = std::nullopt;

    if (cache_.size () == capacity_) {
        victim.emplace (std::move (cache_.back ()));

        hash_map_.erase (victim->first);
        cache_.pop_back ();
    }

    cache_.emplace_front (key, std::move (page));
    hash_map_[key] = cache_.begin ();

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<PageT> LRU<PageT, KeyT>::Extract (const KeyT& key)
{
    auto it = hash_map_.find (key);

    if (it == hash_map_.end ())
        return std::nullopt;

    PageT page = std::move (it->second->second);

    cache_.erase (it->second);
    hash_map_.erase (it);

    return page;
}

} // namespace caches

//--------------------------------------------------------------------------------
