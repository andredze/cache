#pragma once

//--------------------------------------------------------------------------------

#include <unordered_map>
#include <list>
#include <cstddef>
#include <cassert>
#include <optional>

//--------------------------------------------------------------------------------

namespace caches
{

//==================================================
/*
    Cache is implemented under assumption,
    that page size is much bigger than the size of an integer
    (is calculated in KiloBytes)

    Input capacity (or cache initial size) is amount of pages,
    that can be stored.

    2Q caching algorithm sources:
        https://arpitbhayani.me/blogs/2q-cache/
        https://www.vldb.org/conf/1994/P439.PDF
*/
//==================================================

template <typename KeyT>
class LinkedMap {
private:
    using ListIt = std::list<KeyT>::iterator;

    std::list<KeyT> cache_;

    std::unordered_map<KeyT, ListIt> map_;

    void MoveToFront (ListIt iter);

public:
    LinkedMap  () = default;
    ~LinkedMap () = default;

    LinkedMap            (const LinkedMap&) = delete;
    LinkedMap& operator= (const LinkedMap&) = delete;

    LinkedMap            (LinkedMap&&) = default;
    LinkedMap& operator= (LinkedMap&&) = default;

    bool Contains       (const KeyT& key) const;
    bool TryMoveToFront (const KeyT& key);
    void PushFront      (const KeyT& key);
    bool Erase          (const KeyT& key);
    KeyT PopBack        ();

    std::size_t GetSize () const;
};

//--------------------------------------------------------------------------------

template <typename KeyT>
bool LinkedMap<KeyT>::Erase (const KeyT& key)
{
    auto it = map_.find (key);

    if (it == map_.end ()) {
        return false;
    }

    cache_.erase (it->second);
    map_.erase (it);

    return true;
}

//--------------------------------------------------------------------------------

template <typename KeyT>
void LinkedMap<KeyT>::MoveToFront (ListIt iter)
{
    cache_.splice (cache_.begin (), cache_, iter);
}

//--------------------------------------------------------------------------------

template <typename KeyT>
bool LinkedMap<KeyT>::TryMoveToFront (const KeyT& key)
{
    auto map_iter = map_.find (key);

    if (map_iter == map_.end ()) {
        return false;
    }

    MoveToFront (map_iter->second);

    return true;
}

//--------------------------------------------------------------------------------

template <typename KeyT>
void LinkedMap<KeyT>::PushFront (const KeyT& key)
{
    cache_.push_front (key);
    map_[key] = cache_.begin ();
}

//--------------------------------------------------------------------------------

template <typename KeyT>
KeyT LinkedMap<KeyT>::PopBack ()
{
    KeyT key = cache_.back ();
    cache_.pop_back ();

    map_.erase (key);

    return key;
}

//--------------------------------------------------------------------------------

template <typename KeyT>
bool LinkedMap<KeyT>::Contains (const KeyT& key) const
{
    return map_.contains (key);
}

//--------------------------------------------------------------------------------

template <typename KeyT>
std::size_t LinkedMap<KeyT>::GetSize () const
{
    return map_.size ();
}

//==================================================

constexpr double kDefaultPercentageForThresholdIn  = 25;
constexpr double kDefaultPercentageForThresholdOut = 50;

//==================================================

template <
    typename PageT,
    typename KeyT,
    typename LoaderT = PageT (*)(const KeyT&)
>
class TwoQ {
private:
    using Victim = std::pair<KeyT, PageT>;

    LoaderT slow_get_page_;

    std::unordered_map<KeyT, PageT> pages_;

    const std::size_t capacity_;
    const std::size_t size_threshold_in_;
    const std::size_t size_threshold_out_;

    LinkedMap<KeyT> hot_cache_;
    LinkedMap<KeyT> in_cache_;
    LinkedMap<KeyT> out_cache_;

    bool TryHit (const KeyT& key);

public:
    TwoQ (
        std::size_t capacity,
        LoaderT loader,
        std::size_t percentage_in_  = kDefaultPercentageForThresholdIn,
        std::size_t percentage_out_ = kDefaultPercentageForThresholdOut
    ) :
        slow_get_page_(std::move(loader)),
        capacity_          (capacity),
        size_threshold_in_ (capacity * percentage_in_  / 100),
        size_threshold_out_(capacity * percentage_out_ / 100)
    {}

    ~TwoQ () = default;

    TwoQ            (const TwoQ&) = delete;
    TwoQ& operator= (const TwoQ&) = delete;

    TwoQ            (TwoQ&&) = default;
    TwoQ& operator= (TwoQ&&) = default;

    std::size_t GetCapacity         () const;
    std::size_t GetSizeThresholdIn  () const;
    std::size_t GetSizeThresholdOut () const;
    std::size_t GetSize             () const;

    bool IsFull () const;

    bool  Hit     (const KeyT& key);
    PageT GetPage (const KeyT& key);

    std::optional<Victim> Insert (const KeyT& key);
    std::optional<Victim> Insert (const KeyT& key, PageT page);

    std::optional<PageT> Extract (const KeyT& key);
};

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::size_t TwoQ<PageT, KeyT, LoaderT>::GetSizeThresholdIn () const
{
    return size_threshold_in_;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::size_t TwoQ<PageT, KeyT, LoaderT>::GetSizeThresholdOut () const
{
    return size_threshold_out_;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::size_t TwoQ<PageT, KeyT, LoaderT>::GetSize () const
{
    return pages_.size ();
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::size_t TwoQ<PageT, KeyT, LoaderT>::GetCapacity () const
{
    return capacity_;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
bool TwoQ<PageT, KeyT, LoaderT>::IsFull () const
{
    return capacity_ == pages_.size ();
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
bool TwoQ<PageT, KeyT, LoaderT>::TryHit (const KeyT& key)
{
    if (!pages_.contains (key)) {
        return false;
    }

    hot_cache_.TryMoveToFront (key);

    return true;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
bool TwoQ<PageT, KeyT, LoaderT>::Hit (const KeyT& key)
{
    if (TryHit (key)) {
        return true;
    }

    if (capacity_ != 0) {
        Insert (key, slow_get_page_ (key));
    }

    return false;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::optional<typename TwoQ<PageT, KeyT, LoaderT>::Victim>
TwoQ<PageT, KeyT, LoaderT>::Insert (const KeyT& key)
{
    if (capacity_ == 0 || TryHit (key)) {
        return std::nullopt;
    }

    PageT page = slow_get_page_ (key);

    return Insert (key, std::move (page));
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::optional<typename TwoQ<PageT, KeyT, LoaderT>::Victim>
TwoQ<PageT, KeyT, LoaderT>::Insert (const KeyT& key, PageT page)
{
    if (capacity_ == 0 || TryHit (key)) {
        return std::nullopt;
    }

    // Remove the returning key before eviction adds new history.
    const bool was_in_out = out_cache_.Erase (key);

    std::optional<Victim> victim;

    if (IsFull ()) {
        const bool evict_from_in =
            in_cache_.GetSize () > size_threshold_in_
            || hot_cache_.GetSize () == 0;

        KeyT victim_key = evict_from_in
            ? in_cache_.PopBack ()
            : hot_cache_.PopBack ();

        auto victim_it = pages_.find (victim_key);

        assert (victim_it != pages_.end ());

        victim.emplace (victim_key, std::move (victim_it->second));
        pages_.erase (victim_it);

        if (evict_from_in) {
            out_cache_.PushFront (victim_key);

            if (out_cache_.GetSize () > size_threshold_out_) {
                out_cache_.PopBack ();
            }
        }
    }

    pages_.emplace (key, std::move (page));

    if (was_in_out) {
        hot_cache_.PushFront (key);
    }
    else {
        in_cache_.PushFront (key);
    }

    return victim;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT, typename LoaderT>
std::optional<PageT>
TwoQ<PageT, KeyT, LoaderT>::Extract (const KeyT& key)
{
    if (!TryHit (key)) {
        return std::nullopt;
    }

    auto it = pages_.find (key);

    PageT page = std::move (it->second);
    pages_.erase (it);

    if (in_cache_.Erase (key)) {
        out_cache_.PushFront (key);

        if (out_cache_.GetSize () > size_threshold_out_) {
            out_cache_.PopBack ();
        }
    }
    else {
        hot_cache_.Erase (key);
    }

    return page;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT, typename LoaderT>
PageT TwoQ<PageT, KeyT, LoaderT>::GetPage (const KeyT& key)
{
    if (capacity_ == 0) {
        return slow_get_page_ (key);
    }

    Hit (key);

    return pages_.at (key);
}

//--------------------------------------------------------------------------------

}; // namespace caches

//--------------------------------------------------------------------------------
