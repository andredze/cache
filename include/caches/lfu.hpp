#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <new>
#include <unordered_map>

#include "input.hpp"

//--------------------------------------------------------------------------------

namespace caches {

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class LFU {

private:

    using Victim = std::pair<KeyT, PageT>;

//==================================================

struct Node {
    KeyT   key_;
    size_t freq_;
    PageT  value_;

    typename std::list<Node*>::iterator it_;
};

//==================================================

    std::unordered_map<KeyT  , Node*>  key_map_;
    std::unordered_map<size_t, std::list<Node*>> freq_map_;

    size_t size_    ;
    size_t capacity_;
    size_t min_freq_;

//==================================================

    std::optional<Victim> DeleteLowFreqElem ();
    void AddByFreq (size_t freq, Node* node);

    std::optional<Victim> Add (const PageT& page, const KeyT& key);
    void MoveNodeToIncreasedFreq (const KeyT& key);

//==================================================

public:

    LFU (std::size_t input_capacity) :
        size_     (0),
        capacity_ (input_capacity),
        min_freq_ (0)
        {};

    ~LFU () {
        for (auto& [key, node] : key_map_) {
            delete node;
        }
    };

    bool Contains (const KeyT& key) const;
    bool Hit      (const KeyT& key);

    PageT GetPage (const KeyT& key);

    std::optional<Victim> Insert (const KeyT& key);
    std::optional<Victim> Insert (const KeyT& key, PageT page);

    std::optional<PageT> Extract(const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LFU<PageT, KeyT>::Victim>
LFU<PageT, KeyT>::DeleteLowFreqElem ()
{
    std::list<Node*>& list = freq_map_[min_freq_];

    Node* node = list.front ();

    list.pop_front ();
    key_map_.erase (node->key_);

    Victim victim_node {node->key_, std::move (node->value_)};

    delete (node);

    size_--;

    if (list.empty()) {
        freq_map_.erase(min_freq_);
    }

    return victim_node;
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LFU<PageT, KeyT>::AddByFreq (size_t freq, Node* node)
{
    std::list<Node*>& list = freq_map_[freq];

    list.push_back (node);

    node->it_ = std::prev(list.end());
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LFU<PageT, KeyT>::Contains (const KeyT& key) const
{
    return key_map_.contains (key);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
PageT LFU<PageT, KeyT>::GetPage (const KeyT& key)
{
    Hit (key);

    PageT page = key_map_[key]->value_;

    return page;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LFU<PageT, KeyT>::Victim>
LFU<PageT, KeyT>::Add (const PageT& page, const KeyT& key)
{
    Node* node = nullptr;

    try {
        node = new Node {key, 0, page};
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Allocation error\nCant add to cache\n";

        throw ;
    }

    std::optional<Victim> victim_node = std::nullopt;

    if (size_ == capacity_) {
        victim_node = DeleteLowFreqElem ();
    }

    key_map_[key] = node;

    AddByFreq (0, node);
    min_freq_ = 0;

    size_++;

    return victim_node;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LFU<PageT, KeyT>::MoveNodeToIncreasedFreq (const KeyT& key)
{
    Node* node = key_map_[key];

    size_t old_freq = node->freq_;

    std::list<Node*>& list = freq_map_[old_freq];

    list.erase (node->it_);

    if (list.empty ()) {
        freq_map_.erase (old_freq);

        if (min_freq_ == old_freq) {
            min_freq_++;
        }
    }

    node->freq_++;

    AddByFreq (node->freq_, node);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LFU<PageT, KeyT>::Hit (const KeyT& key)
{
    if (capacity_ == 0) {
        return false;
    }

    if (Contains (key) == false) {
        PageT page = GetSlowPage<PageT,KeyT>(key);

        Add (page, key);

        return false;
    }

    PageT page = key_map_[key]->value_;

    MoveNodeToIncreasedFreq (key);

    return true;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LFU<PageT, KeyT>::Victim>
LFU<PageT, KeyT>::Insert (const KeyT& key, PageT page)
{
    if (capacity_ == 0) {
        return false;
    }

    if (Contains (key) == false) {
        return Add (page, key);
    }

    PageT page = key_map_[key]->value_;

    MoveNodeToIncreasedFreq (key);

    return nullopt;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LFU<PageT, KeyT>::Victim>
LFU<PageT, KeyT>::Insert (const KeyT& key)
{
    PageT page = GetSlowPage (key);

    return Insert (key, page);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<PageT>
LFU<PageT, KeyT>::Extract (const KeyT& key)
{
    auto it = key_map_.find (key);

    if (it == key_map_.end ()) {
        return std::nullopt;
    }

    Node* node = it->second;

    PageT page = std::move (node->value_);

    size_t freq = node->freq_;

    auto freq_it = freq_map_.find (freq);

    freq_it->second.erase (node->it_);

    if (freq_it->second.empty ()) {
        freq_map_.erase (freq_it);
    }

    key_map_.erase (it);

    delete node;
    size_--;

    if (freq_map_.empty ()) {
        min_freq_ = 0;
    }
    else if (freq == min_freq_ && !freq_map_.contains (freq)) {
        min_freq_ = freq_map_.begin ()->first;

        for (const auto& [f, list] : freq_map_) {
            if (f < min_freq_) {
                min_freq_ = f;
            }
        }
    }

    return page;
}

} // namespace caches

//--------------------------------------------------------------------------------
