#pragma once

//--------------------------------------------------------------------------------

#include <unordered_map>
#include <list>
#include <cstddef>

//--------------------------------------------------------------------------------

namespace caches
{

//==================================================
/*
    Cache is implemented with assumption, 
    that cache_page size is much bigger than the size of an integer 
    (is calculated in KiloBytes)
    Input capacity (or cache initial size) is amount of pages, 
    that can be stored.

    2Q caching algorithm sources: 
        https://arpitbhayani.me/blogs/2q-cache/
        https://www.vldb.org/conf/1994/P439.PDF
*/

//--------------------------------------------------------------------------------

template <typename KeyT>
class LinkedMap {
private:
    using ListIt = std::list<KeyT>::iterator;

    std::list<KeyT> cache_;

    std::unordered_map<KeyT, ListIt> map_;

    void MoveToFront (ListIt iter);

public:
    bool TryMoveToFront (KeyT key);

    void PushFront (KeyT node);

    KeyT PopBack ();

    bool Contains (KeyT key);

    std::size_t GetSize ();
};

//--------------------------------------------------------------------------------

template <typename KeyT>
void LinkedMap<KeyT>::MoveToFront (ListIt iter)
{
    cache_.splice (cache_.begin (), cache_, iter);
}

//--------------------------------------------------------------------------------

template <typename KeyT>
bool LinkedMap<KeyT>::TryMoveToFront (KeyT key)
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
void LinkedMap<KeyT>::PushFront (KeyT key)
{
    cache_.push_front (key);
    map_[key] = cache_.begin ();
}

//--------------------------------------------------------------------------------

template <typename KeyT>
KeyT LinkedMap<KeyT>::PopBack ()
{
    KeyT key = cache_.back ();
    map_.erase (key);

    return key;
}

//--------------------------------------------------------------------------------

template <typename KeyT>
bool LinkedMap<KeyT>::Contains (KeyT key)
{
    return map_.contains (key);
}

//--------------------------------------------------------------------------------

template <typename KeyT>
std::size_t LinkedMap<KeyT>::GetSize ()
{
    return map_.size ();
}

//==================================================

constexpr double kDefaultPercentageForThresholdIn  = 25;
constexpr double kDefaultPercentageForThresholdOut = 50;

//==================================================

template <typename PageT, typename KeyT>
class TwoQCache {

private:
    std::unordered_map<KeyT, PageT> pages_;

    const std::size_t capacity_;
    const std::size_t size_threshold_in_;
    const std::size_t size_threshold_out_;

    LinkedMap<KeyT> hot_cache_;
    LinkedMap<KeyT> in_cache_;
    LinkedMap<KeyT> out_cache_;

    void StorePageInMemory (KeyT key, PageT& page);

public:
    TwoQCache (
        std::size_t capacity, 
        double      percentage_threshold_in_  = kDefaultPercentageForThresholdIn, 
        double      percentage_threshold_out_ = kDefaultPercentageForThresholdOut
    ) :
        capacity_          (capacity),
        size_threshold_in_ (capacity * percentage_threshold_in_  / 100),
        size_threshold_out_(capacity * percentage_threshold_out_ / 100)
    {}

    ~TwoQCache () = default;

    std::size_t GetCapacity () const;
    std::size_t GetSizeThresholdIn () const;
    std::size_t GetSizeThresholdOut () const;

    std::size_t GetSize () const;

    bool IsFull () const;

    template <typename FuncT>
    PageT Access (KeyT key, FuncT slow_get_page);
};

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
std::size_t TwoQCache<PageT, KeyT>::GetSize () const
{
    return pages_.size ();
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
std::size_t TwoQCache<PageT, KeyT>::GetCapacity () const
{
    return capacity_;
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool TwoQCache<PageT, KeyT>::IsFull () const
{
    return capacity_ == pages_.size ();
}

//————————————————————————————————————————————————————————————————————————————————

/*
Algorithm reference:

// If there is space, we give it to X.
// If there is no space, we free a page slot to
// make room for page X.
reclaimfor(page X)
begin
    if there are free page slots then
        put X into a free page slot

    else if (sizeof[A1in] > Kin)
        page out the tail of A1in, call it Y
        add identifier of Y to the head of A1out

        if (sizeof[A1out] > Kout)
            remove identifier of Z from
            the tail of A1out
        end if

        put X into the reclaimed page slot

    else
        page out the tail of Am, call it Y
        // do not put it on A1out; it hasn’t been
        // accessed for a while

        put X into the reclaimed page slot
    end if
end
*/

template <typename PageT, typename KeyT>
void TwoQCache<PageT, KeyT>::StorePageInMemory (KeyT key, PageT& page)
{
    if (!IsFull ()) {
        pages_[key] = page;
        return;
    }

    // else if
    if (in_cache_.GetSize () > size_threshold_in_) {
        KeyT victim = in_cache_.PopBack ();
        pages_.erase (victim);

        out_cache_.PushFront (victim);

        if (out_cache_.GetSize () > size_threshold_out_) {
            out_cache_.PopBack ();
        }

        pages_[key] = page;

        return;
    }

    // else
    KeyT victim = hot_cache_.PopBack ();
    pages_.erase (victim);

    // do not put in out_cache_

    pages_[key] = page;
}

//————————————————————————————————————————————————————————————————————————————————

/*
Algorithm reference:

On accessing a page X:
begin
    if X is in Am then
        move X to the head of Am
    else if (X is in A1out) then
        reclaimfor
        add X to the head of Am
    else if (X is in A1in) // do nothing
    else // X is in no queue
        reclaimfor
        add X to the head of A1in
    end if
end

*/
template <typename PageT, typename KeyT>
template <typename FuncT>
PageT TwoQCache<PageT, KeyT>::Access (KeyT key, FuncT slow_get_page)
{
    // if key is in the hot_cache
    if (hot_cache_.TryMoveToFront (key)) {
        return pages_.at(key);
    }

    if (out_cache_.Contains (key)) {
        PageT page = slow_get_page (key);
        StorePageInMemory (key, page);

        hot_cache_.PushFront (key);

        assert (pages_.contains(key));
        return page;
    }

    if (in_cache_.Contains (key)) {
        // do nothing;
        return pages_.at(key);
    }

    PageT page = slow_get_page (key);
    StorePageInMemory (key, page);

    in_cache_.PushFront (key);

    return page;
}

//--------------------------------------------------------------------------------

}; // namespace caches

//--------------------------------------------------------------------------------