#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <iterator>
#include <list>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "input.hpp"

//--------------------------------------------------------------------------------

namespace caches {

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class LIRS {

public:

    using Victim = std::pair<KeyT, PageT>;

private:

    //==================================================

    struct Node {
        KeyT key_;
        std::optional<PageT> value_;

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

    void UpdateLIRPage   (const KeyT& key);
    void UpdateHirPage   (const KeyT& key);
    void UpgradeHIRToLIR (const KeyT& key);

    std::optional<Victim> RestoreNonResidentHIR (const KeyT& key, PageT page);
    std::optional<Victim> AddNewPage            (const KeyT& key, PageT page);

    Victim EvictHIR ();

    void PruneStack ();
    void PromoteHIRAfterExtract ();

    //==================================================

public:

    LIRS (std::size_t lir_size, std::size_t hir_size) :
        size_         (0),
        capacity_     (lir_size + hir_size),
        lir_capacity_ (lir_size)
    {
        if (lir_size == 0 || hir_size == 0) {
            throw std::invalid_argument ("LIR and HIR sizes must be positive");
        }
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

    std::optional<Victim> Insert (const KeyT& key);
    std::optional<Victim> Insert (const KeyT& key, PageT page);

    std::optional<PageT> Extract (const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool LIRS<PageT, KeyT>::Contains (const KeyT& key) const
{
    auto it = key_map_.find (key);

    return it != key_map_.end () && it->second->is_resident_;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
PageT LIRS<PageT, KeyT>::GetPage (const KeyT& key)
{
    Hit (key);

    return *key_map_.at (key)->value_;
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
        AddNewPage (key, GetSlowPage<PageT, KeyT> (key));

        return false;
    }

    //==================================================

    Node* node = it->second;

    if (node->is_resident_ == false) {
        RestoreNonResidentHIR (key, GetSlowPage<PageT, KeyT> (key));

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
typename LIRS<PageT, KeyT>::Victim
LIRS<PageT, KeyT>::EvictHIR ()
{
    Node* node = queue_.back ();

    Victim victim (node->key_, std::move (*node->value_));

    node->value_.reset ();

    queue_.pop_back ();

    node->in_q_ = false;
    node->is_resident_ = false;

    size_--;

    if (node->in_s_ == false) {
        key_map_.erase (node->key_);
        delete node;
    }

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LIRS<PageT, KeyT>::Victim>
LIRS<PageT, KeyT>::RestoreNonResidentHIR (const KeyT& key, PageT page)
{
    Node* new_lir_node = key_map_.at (key);

    std::optional<Victim> victim;

    if (size_ == capacity_) {
        victim.emplace (EvictHIR ());
    }

    new_lir_node->value_ = std::move (page);
    new_lir_node->is_resident_ = true;
    new_lir_node->is_lir_ = true;

    size_++;

    //==================================================

    stack_.splice (stack_.end (), stack_, new_lir_node->s_it_);

    //==================================================

    if (size_ > lir_capacity_) {
        Node* displaced_node = stack_.front ();

        displaced_node->is_lir_ = false;

        queue_.push_front (displaced_node);

        displaced_node->q_it_ = queue_.begin ();
        displaced_node->in_q_ = true;
    }

    //==================================================

    PruneStack ();

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LIRS<PageT, KeyT>::Victim>
LIRS<PageT, KeyT>::AddNewPage (const KeyT& key, PageT page)
{
    Node* node = new Node {
        key, std::move (page),
        false, true, false, false, {}, {}
    };

    std::optional<Victim> victim;

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
            victim.emplace (EvictHIR ());
        }

        stack_.push_back (node);

        node->s_it_ = std::prev (stack_.end ());
        node->in_s_ = true;

        queue_.push_front (node);

        node->q_it_ = queue_.begin ();
        node->in_q_ = true;
    }

    //==================================================

    key_map_[key] = node;

    size_++;

    return victim;
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

template <typename PageT, typename KeyT>
void LIRS<PageT, KeyT>::PromoteHIRAfterExtract ()
{
    if (queue_.empty ()) {
        return;
    }

    Node* node = queue_.back ();

    queue_.pop_back ();

    node->in_q_ = false;
    node->is_lir_ = true;

    if (node->in_s_ == false) {
        stack_.push_front (node);

        node->s_it_ = stack_.begin ();
        node->in_s_ = true;
    }
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LIRS<PageT, KeyT>::Victim>
LIRS<PageT, KeyT>::Insert (const KeyT& key)
{
    if (Contains (key)) {
        Hit (key);

        return std::nullopt;
    }

    PageT page = GetSlowPage<PageT, KeyT> (key);

    return Insert (key, std::move (page));
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename LIRS<PageT, KeyT>::Victim>
LIRS<PageT, KeyT>::Insert (const KeyT& key, PageT page)
{
    auto it = key_map_.find (key);

    if (it == key_map_.end ()) {
        return AddNewPage (key, std::move (page));
    }

    if (it->second->is_resident_) {
        Hit (key);

        return std::nullopt;
    }

    return RestoreNonResidentHIR (key, std::move (page));
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<PageT> LIRS<PageT, KeyT>::Extract (const KeyT& key)
{
    auto it = key_map_.find (key);

    if (it == key_map_.end () || !it->second->is_resident_) {
        return std::nullopt;
    }

    Node* node = it->second;

    PageT page = std::move (*node->value_);

    node->value_.reset ();

    bool was_lir = node->is_lir_;

    //==================================================

    if (node->in_q_) {
        queue_.erase (node->q_it_);

        node->in_q_ = false;
    }

    node->is_lir_ = false;
    node->is_resident_ = false;

    size_--;

    //==================================================

    if (was_lir && size_ >= lir_capacity_) {
        PromoteHIRAfterExtract ();
    }

    //==================================================

    if (node->in_s_ == false) {
        key_map_.erase (it);
        delete node;
    }

    PruneStack ();

    return page;
}

//--------------------------------------------------------------------------------

} // namespace caches

//--------------------------------------------------------------------------------