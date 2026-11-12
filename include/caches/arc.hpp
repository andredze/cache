
#pragma once

//--------------------------------------------------------------------------------

#include <algorithm>
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
class ARC {

public:

    using Victim = std::pair<KeyT, PageT>;

private:

    //==================================================

    enum class ListType {
        Recent,
        Frequent,
        RecentHistory,
        FrequentHistory
    };

    //==================================================

    struct Node {
        ListType type_;

        typename std::list<KeyT>::iterator it_;
    };

    //==================================================

    std::list<KeyT> recent_;
    std::list<KeyT> frequent_;

    std::list<KeyT> recent_history_;
    std::list<KeyT> frequent_history_;

    //==================================================

    std::unordered_map<KeyT, Node*> key_map_;
    std::unordered_map<KeyT, PageT> pages_;

    //==================================================

    size_t size_;
    size_t capacity_;
    size_t recent_target_;

    //==================================================

    void UpdateRecentPage   (const KeyT& key);
    void UpdateFrequentPage (const KeyT& key);

    std::optional<Victim> RestoreRecentPage   (const KeyT& key, PageT page);
    std::optional<Victim> RestoreFrequentPage (const KeyT& key, PageT page);

    std::optional<Victim> AddNewPage (const KeyT& key, PageT page);
    std::optional<Victim> Replace    (const KeyT& key);

    void RemoveHistoryPage (ListType type);

    //==================================================

public:

    ARC (std::size_t capacity) :
        size_          (0),
        capacity_      (capacity),
        recent_target_ (0)
    {
        if (capacity == 0) {
            throw std::invalid_argument ("Capacity must be positive");
        }
    }

    ~ARC ()
    {
        for (auto& [key, node] : key_map_) {
            delete node;
        }
    }

    ARC (const ARC&) = delete;
    ARC& operator= (const ARC&) = delete;

    //==================================================

    bool Contains (const KeyT& key) const;
    bool Hit      (const KeyT& key);
    PageT GetPage (const KeyT& key);

    std::optional<Victim> Insert (const KeyT& key);
    std::optional<Victim> Insert (const KeyT& key, PageT page);

    std::optional<PageT> Extract (const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool ARC<PageT, KeyT>::Contains (const KeyT& key) const
{
    return pages_.contains (key);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void ARC<PageT, KeyT>::UpdateRecentPage (const KeyT& key)
{
    Node* node = key_map_.at (key);

    frequent_.splice (frequent_.begin (), recent_, node->it_);

    node->type_ = ListType::Frequent;
    node->it_   = frequent_.begin ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void ARC<PageT, KeyT>::UpdateFrequentPage (const KeyT& key)
{
    Node* node = key_map_.at (key);

    frequent_.splice (frequent_.begin (), frequent_, node->it_);

    node->it_ = frequent_.begin ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void ARC<PageT, KeyT>::RemoveHistoryPage (ListType type)
{
    auto& history = (type == ListType::RecentHistory) ? recent_history_ : frequent_history_;

    if (history.empty ()) {
        return;
    }

    const KeyT& key = history.back ();

    Node* node = key_map_.at (key);

    delete node;
    key_map_.erase (key);

    history.pop_back ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename ARC<PageT, KeyT>::Victim>
ARC<PageT, KeyT>::Replace (const KeyT& key)
{
    auto it = key_map_.find (key);

    bool in_frequent_history = it != key_map_.end () &&
        it->second->type_ == ListType::FrequentHistory;

    bool condition = !recent_.empty () && (recent_.size () > recent_target_ ||
        (in_frequent_history && recent_.size () == recent_target_));

    if (!condition && frequent_.empty ()) {
        condition = true;
    }

    auto& source  = condition ? recent_         : frequent_;
    auto& history = condition ? recent_history_ : frequent_history_;

    ListType type = condition ? ListType::RecentHistory : ListType::FrequentHistory;

    if (source.empty ()) {
        return std::nullopt;
    }

    auto last = std::prev (source.end ());

    const KeyT victim_key = *last;

    Victim victim (victim_key, std::move (pages_.at (victim_key)));
    pages_.erase (victim_key);

    Node* node = key_map_.at (victim_key);

    history.splice (history.begin (), source, last);

    node->type_ = type;
    node->it_   = history.begin ();

    size_--;

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename ARC<PageT, KeyT>::Victim>
ARC<PageT, KeyT>::AddNewPage (const KeyT& key, PageT page)
{
    size_t recent_total = recent_.size () + recent_history_.size ();

    size_t all_total = recent_total + frequent_.size () + frequent_history_.size ();

    std::optional<Victim> victim;

    if (recent_total == capacity_) {

        if (recent_.size () < capacity_) {
            RemoveHistoryPage (ListType::RecentHistory);

            if (size_ == capacity_) {
                victim = Replace (key);
            }
        }
        else {
            const KeyT victim_key = recent_.back ();

            victim.emplace (victim_key, std::move (pages_.at (victim_key)));
            pages_.erase (victim_key);

            Node* node = key_map_.at (victim_key);

            delete node;
            key_map_.erase (victim_key);

            recent_.pop_back ();
            size_--;
        }
    }
    else if (recent_total < capacity_ && all_total >= capacity_) {

        if (all_total == 2 * capacity_) {
            RemoveHistoryPage (ListType::FrequentHistory);
        }

        if (size_ == capacity_) {
            victim = Replace (key);
        }
    }

    recent_.push_front (key);

    Node* node = new Node {ListType::Recent, recent_.begin ()};

    key_map_.emplace (key, node);
    pages_.emplace (key, std::move (page));

    size_++;

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename ARC<PageT, KeyT>::Victim>
ARC<PageT, KeyT>::RestoreRecentPage (const KeyT& key, PageT page)
{
    size_t delta = std::max<size_t> (1, frequent_history_.size () / recent_history_.size ());

    recent_target_ = std::min (capacity_, recent_target_ + delta);

    std::optional<Victim> victim;

    if (size_ == capacity_) {
        victim = Replace (key);
    }

    Node* node = key_map_.at (key);

    frequent_.splice (frequent_.begin (), recent_history_, node->it_);

    node->type_ = ListType::Frequent;
    node->it_   = frequent_.begin ();

    pages_.emplace (key, std::move (page));

    size_++;

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename ARC<PageT, KeyT>::Victim>
ARC<PageT, KeyT>::RestoreFrequentPage (const KeyT& key, PageT page)
{
    size_t delta = std::max<size_t> (1, recent_history_.size () / frequent_history_.size ());

    recent_target_ = (delta >= recent_target_) ? 0 : recent_target_ - delta;

    std::optional<Victim> victim;

    if (size_ == capacity_) {
        victim = Replace (key);
    }

    Node* node = key_map_.at (key);

    frequent_.splice (frequent_.begin (), frequent_history_, node->it_);

    node->type_ = ListType::Frequent;
    node->it_   = frequent_.begin ();

    pages_.emplace (key, std::move (page));

    size_++;

    return victim;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool ARC<PageT, KeyT>::Hit (const KeyT& key)
{
    auto it = key_map_.find (key);

    if (it == key_map_.end ()) {
        AddNewPage (key, GetSlowPage<PageT, KeyT> (key));

        return false;
    }

    switch (it->second->type_) {

        case ListType::Recent:
            UpdateRecentPage (key);

            return true;

        case ListType::Frequent:
            UpdateFrequentPage (key);

            return true;

        case ListType::RecentHistory:
            RestoreRecentPage (key, GetSlowPage<PageT, KeyT> (key));

            return false;

        case ListType::FrequentHistory:
            RestoreFrequentPage (key, GetSlowPage<PageT, KeyT> (key));

            return false;
    }

    return false;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
PageT ARC<PageT, KeyT>::GetPage (const KeyT& key)
{
    Hit (key);

    return pages_.at (key);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<typename ARC<PageT, KeyT>::Victim>
ARC<PageT, KeyT>::Insert (const KeyT& key)
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
std::optional<typename ARC<PageT, KeyT>::Victim>
ARC<PageT, KeyT>::Insert (const KeyT& key, PageT page)
{
    auto it = key_map_.find (key);

    if (it == key_map_.end ()) {
        return AddNewPage (key, std::move (page));
    }

    switch (it->second->type_) {

        case ListType::Recent:
            UpdateRecentPage (key);
            return std::nullopt;

        case ListType::Frequent:
            UpdateFrequentPage (key);
            return std::nullopt;

        case ListType::RecentHistory:
            return RestoreRecentPage (key, std::move (page));

        case ListType::FrequentHistory:
            return RestoreFrequentPage (key, std::move (page));
    }

    return std::nullopt;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<PageT> ARC<PageT, KeyT>::Extract (const KeyT& key)
{
    auto it = key_map_.find (key);

    if (it == key_map_.end ()) {
        return std::nullopt;
    }

    Node* node = it->second;

    if (node->type_ != ListType::Recent && node->type_ != ListType::Frequent) {
        return std::nullopt;
    }

    PageT page = std::move (pages_.at (key));
    pages_.erase (key);

    if (node->type_ == ListType::Recent) {
        recent_history_.splice (recent_history_.begin (), recent_, node->it_);

        node->type_ = ListType::RecentHistory;
        node->it_   = recent_history_.begin ();
    }
    else {
        frequent_history_.splice (frequent_history_.begin (), frequent_, node->it_);

        node->type_ = ListType::FrequentHistory;
        node->it_   = frequent_history_.begin ();
    }

    size_--;

    return page;
}

//--------------------------------------------------------------------------------

} // namespace caches

//--------------------------------------------------------------------------------
