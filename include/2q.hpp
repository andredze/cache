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
    (about a few Kbytes)
    Input capacity (or cache initial size) is amount of pages, 
    that can be stored.
*/
/*
    2Q caching algorithm

    Sources: 
        https://arpitbhayani.me/blogs/2q-cache/
        https://www.vldb.org/conf/1994/P439.PDF

    / If there is space, we give it to X.
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

    On accessing a page X :
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

//--------------------------------------------------------------------------------

template <typename NodeT>
class LinkedMap {
private:
    using ListIt = std::list<NodeT>::iterator;

    std::list<NodeT> cache_;

    std::unordered_map<NodeT, ListIt> map_;

public:
    // void MoveToFront (NodeT node);

    // void Remove (NodeT node);

    // bool IsFull ();

    // void PopBack ();

    std::size_t GetSize ();
};

//==================================================

constexpr double kDefaultPercentageForThresholdIn  = 0.25;
constexpr double kDefaultPercentageForThresholdOut = 0.50;

//==================================================

template <typename PageT, typename KeyT>
class TwoQCache {

private:
    std::unordered_map<KeyT, PageT> pages_;

    const std::size_t capacity_;
    const std::size_t size_threshold_in_;
    const std::size_t size_threshold_out_;

    LinkedMap<KeyT> hot_cache_;
    LinkedMap<KeyT> in_cache;
    LinkedMap<KeyT> out_cache;

    void PutPageInMemory (KeyT key, PageT& page);

public:
    TwoQCache (
        std::size_t capacity, 
        double      percentage_threshold_in_  = kDefaultPercentageForThresholdIn, 
        double      percentage_threshold_out_ = kDefaultPercentageForThresholdOut
    ) :
        capacity_          (capacity),
        size_threshold_in_ (capacity * percentage_threshold_in_),
        size_threshold_out_(capacity * percentage_threshold_out_)
    {}

    ~TwoQCache ();

    PageT Access (KeyT key);
};

//--------------------------------------------------------------------------------

}; // namespace caches

//--------------------------------------------------------------------------------