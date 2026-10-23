#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>

#include "cache.hpp"

//--------------------------------------------------------------------------------

namespace lirs {

template <typename PageT, typename KeyT>
class LIRS {

private:

    //==================================================

    struct Node {
        KeyT key_;
        PageT value_;

        bool is_lir_;
        bool is_resident_;
        bool in_s_;
        bool in_q_;

        typename std::list<Node*>::iterator s_it_;
        typename std::list<Node*>::iterator q_it_;
    };

    //==================================================

    std::unordered_map<KeyT, Node*> key_map_;

    std::list<Node*> stack_;
    std::list<Node*> queue_;

    size_t size_;
    size_t capacity_;
    size_t lir_capacity_;

    //==================================================

    void UpdateLIRPage         (const KeyT& key);
    void UpdateHirPage         (const KeyT& key);
    void UpgradeHIRToLIR       (const KeyT& key);
    void RestoreNonResidentHIR (const KeyT& key);
    void AddNewPage            (const KeyT& key);

    void EvictHIR   ();
    void PruneStack ();

    //==================================================

public:

    explicit LIRS (std::size_t input_capacity) :
        size_         (0),
        capacity_     (input_capacity),
        lir_capacity_ (input_capacity > 1 ? input_capacity - 1 : 0)
    {
    }

    ~LIRS ()
    {
        for (auto& [key, node] : key_map_) {
            delete node;
        }
    }

    LIRS (const LIRS&) = delete;
    LIRS& operator= (const LIRS&) = delete;

    bool Contains (const KeyT& key) const;
    bool Hit      (const KeyT& key);
    PageT GetPage (const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LIRS<PageT, KeyT>::Contains (const KeyT& key) const
{
    return key_map_.contains (key);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
PageT LIRS<PageT, KeyT>::GetPage (const KeyT& key)
{
    if (capacity_ == 0) {
        return GetSlowPage<PageT, KeyT> (key);
    }

    Hit (key);

    return key_map_.at (key)->value_;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LIRS<PageT, KeyT>::Hit (const KeyT& key)
{
    if (capacity_ == 0) {
        return false;
    }

    //==================================================

    auto it = key_map_.find (key);

    if (it == key_map_.end ()) {
        AddNewPage (key);

        return false;
    }

    //==================================================

    Node* node = it->second;

    if (node->is_resident_ == false) {
        RestoreNonResidentHIR (key);

        return false;
    }

    if (lir_capacity_ == 0) {
        queue_.splice (queue_.begin (), queue_, node->q_it_);

        return true;
    }

    //==================================================

    if (node->is_lir_) {
        UpdateLIRPage (key);
    }
    else if (node->in_s_) {
        UpgradeHIRToLIR (key);
    }
    else {
        UpdateHirPage (key);
    }

    return true;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::UpdateLIRPage (const KeyT& key)
{
    Node* node = key_map_.at (key);

    stack_.splice (stack_.end (), stack_, node->s_it_);

    PruneStack ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::UpgradeHIRToLIR (const KeyT& key)
{
    //==================================================
    // HIR -> LIR

    Node* new_lir_node = key_map_.at (key);

    queue_.erase (new_lir_node->q_it_);

    new_lir_node->in_q_ = false;

    stack_.splice (stack_.end (), stack_, new_lir_node->s_it_);

    new_lir_node->is_lir_ = true;

    //==================================================
    // LIR -> HIR

    Node* displaced_node = stack_.front ();

    displaced_node->is_lir_ = false;

    queue_.push_front (displaced_node);

    displaced_node->q_it_ = queue_.begin ();
    displaced_node->in_q_ = true;

    //==================================================

    PruneStack ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::UpdateHirPage (const KeyT& key)
{
    Node* node = key_map_.at (key);

    queue_.splice (queue_.begin (), queue_, node->q_it_);

    stack_.push_back (node);

    node->s_it_ = std::prev (stack_.end ());
    node->in_s_ = true;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::EvictHIR ()
{
    Node* node = queue_.back ();

    queue_.pop_back ();

    node->in_q_        = false;
    node->is_resident_ = false;

    size_--;

    if (node->in_s_ == false) {
        key_map_.erase (node->key_);
        delete node;
    }
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::RestoreNonResidentHIR (const KeyT& key)
{
    Node* new_lir_node = key_map_.at (key);

    PageT page = GetSlowPage<PageT, KeyT> (key);

    //==================================================

    EvictHIR ();

    new_lir_node->value_       = std::move (page);
    new_lir_node->is_resident_ = true;
    new_lir_node->is_lir_      = true;

    size_++;

    //==================================================

    stack_.splice (
        stack_.end (),
        stack_,
        new_lir_node->s_it_
    );

    //==================================================

    Node* displaced_node = stack_.front ();

    displaced_node->is_lir_ = false;

    queue_.push_front (displaced_node);

    displaced_node->q_it_ = queue_.begin ();
    displaced_node->in_q_ = true;

    //==================================================

    PruneStack ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::AddNewPage (const KeyT& key)
{
    if (capacity_ == 0) {
        return;
    }

    Node* node = new Node {
        key,
        GetSlowPage<PageT, KeyT> (key),
        false,
        true,
        false,
        false,
        {},
        {}
    };

    //==================================================

    if (size_ < lir_capacity_) {
        node->is_lir_ = true;

        stack_.push_back (node);

        node->s_it_ = std::prev (stack_.end ());
        node->in_s_ = true;
    }

    //==================================================

    else {
        if (size_ == capacity_) {
            EvictHIR ();
        }

        if (lir_capacity_ != 0) {
            stack_.push_back (node);

            node->s_it_ = std::prev (stack_.end ());
            node->in_s_ = true;
        }

        queue_.push_front (node);

        node->q_it_ = queue_.begin ();
        node->in_q_ = true;
    }

    //==================================================

    key_map_[key] = node;

    size_++;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::PruneStack ()
{
    while (!stack_.empty () && !stack_.front ()->is_lir_) {
        Node* node = stack_.front ();

        stack_.pop_front ();

        node->in_s_ = false;

        if (node->is_resident_ == false) {
            key_map_.erase (node->key_);
            delete node;
        }
    }
}

//--------------------------------------------------------------------------------

} // namespace lirs

//--------------------------------------------------------------------------------
