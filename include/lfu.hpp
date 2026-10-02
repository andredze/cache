#pragma once

//--------------------------------------------------------------------------------

#include "cache.hpp"

//--------------------------------------------------------------------------------

namespace lfu {

template <typename PageT, typename KeyT>
class LFU {

private:

struct Node {
    KeyT   key_;
    size_t freq_;
    PageT  value_;

    typename std::list<Node*>::iterator it_;
};

    std::unordered_map<KeyT  , Node*>  key_map_;
    std::unordered_map<size_t, std::list<Node*>> freq_map_;

    size_t size_    ;
    size_t capacity_;
    size_t min_freq_;

private:

    void DeleteLowFreqElem ();
    void AddByFreq (size_t freq, Node* node);

    bool Add (const PageT& page, const KeyT& key);
    void MoveNodeToIncreasedFreq (const KeyT& key);

public:

    LFU (std::size_t input_capacity) : 
        size_     (0),
        capacity_ (input_capacity),
        min_freq_ (0) {

        };

    ~LFU () {
        for (auto& [key, node] : key_map_) {
            delete node;
        }
    };

    bool Contains (const KeyT& key) const;
    bool Access   (const KeyT& key);
    
    PageT GetPage (const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LFU<PageT, KeyT>::DeleteLowFreqElem ()
{
    std::list<Node*>& list = freq_map_[min_freq_];

    Node* node = list.front ();

    list.pop_front ();
    key_map_.erase (node->key_);

    delete (node);

    size_--;

    if (list.empty()) {
        freq_map_.erase(min_freq_);
    }
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
    if (capacity_ == 0) {
        return PageT {};
    }

    if (Contains (key) == false) {
        PageT page = GetSlowPage<PageT,KeyT>(key);

        Add (page, key);

        return page;
    }

    PageT page = key_map_[key]->value_;

    MoveNodeToIncreasedFreq (key);

    return page;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LFU<PageT, KeyT>::Add (const PageT& page, const KeyT& key)
{
    Node* node = nullptr;

    try {
        node = new Node {key, 0, page};
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Allocation error\nCant add to cache\n";

        throw ;
    }

    if (size_ == capacity_) {
        DeleteLowFreqElem ();
    }

    key_map_[key] = node;

    AddByFreq (0, node);
    min_freq_ = 0;

    size_++;

    return true;
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
bool LFU<PageT, KeyT>::Access (const KeyT& key)
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

void Test();

} // namespace lfu

//--------------------------------------------------------------------------------