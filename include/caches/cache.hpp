#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <deque>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <variant>
#include <optional>

#include "arc.hpp"
#include "lru.hpp"
#include "lfu.hpp"
#include "lirs.hpp"
#include "2q.hpp"

//--------------------------------------------------------------------------------

namespace caches {

//--------------------------------------------------------------------------------

enum class Policy {
    LRU,
    LFU,
    LIRS,
    ARC,
    TwoQ
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class Algorithm {

public:
    using Victim = std::pair<KeyT, PageT>;
    using Loader = std::function<PageT (const KeyT&)>;

private:
    using Storage = std::variant<
        caches::ARC <PageT, KeyT>,
        caches::LRU <PageT, KeyT>,
        caches::LFU <PageT, KeyT>,
        caches::LIRS<PageT, KeyT>,
        caches::TwoQ<PageT, KeyT>
    >;

    Storage storage_;

    static Storage MakeStorage (Policy        policy,
                                std::size_t   capacity,
                                std::size_t   hir_size,
                                const Loader& loader)
    {
        if (capacity == 0) {
            throw std::invalid_argument ("Cache capacity must be positive");
        }

        switch (policy)
        {
        case Policy::LRU:
            return Storage (std::in_place_type<caches::LRU<PageT, KeyT>>,
                            capacity, loader);

        case Policy::LFU:
            return Storage (std::in_place_type<caches::LFU<PageT, KeyT>>,
                            capacity, loader);

        case Policy::ARC:
            return Storage (std::in_place_type<caches::ARC<PageT, KeyT>>,
                            capacity, loader);

        case Policy::TwoQ:
            return Storage (std::in_place_type<caches::TwoQ<PageT, KeyT>>,
                            capacity, loader);

        case Policy::LIRS:
            if (hir_size == 0 || hir_size >= capacity) {
                throw std::invalid_argument ("LIRS requires 0 < HIR size < capacity");
            }

            return Storage (std::in_place_type<caches::LIRS<PageT, KeyT>>,
                            capacity - hir_size, hir_size, loader);

        default:
            throw std::invalid_argument ("Unknown cache policy");
        }
    }

public:

    Algorithm (Policy        policy,
               std::size_t   capacity,
               std::size_t   hir_size,
               const Loader& loader)
    : storage_ (MakeStorage (policy, capacity, hir_size, loader))
    {};

    bool Contains (const KeyT& key);
    bool Hit      (const KeyT& key);

    std::optional<Victim> Insert  (const KeyT& key, PageT page);
    std::optional<PageT>  Extract (const KeyT& key);
};

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool Algorithm<PageT, KeyT>::Contains (const KeyT& key)
{
    return std::visit ([&](auto & cache) {
                return cache.Contains (key);
            },
            storage_);
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
bool Algorithm<PageT, KeyT>::Hit (const KeyT& key)
{
    return std::visit ([&](auto & cache) {
                return cache.Hit (key);
            },
            storage_);
}

//————————————————————————————————————————————————————————————————————————————————

template <typename PageT, typename KeyT>
std::optional<typename Algorithm<PageT, KeyT>::Victim>
Algorithm<PageT, KeyT>::Insert (const KeyT& key, PageT page)
{
    return std::visit ([&](auto & cache) {
                return cache.Insert (key, page);
            },
            storage_);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
std::optional<PageT>
Algorithm<PageT, KeyT>::Extract (const KeyT& key)
{
    return std::visit ([&](auto & cache) {
                return cache.Extract (key);
            },
            storage_);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class MultiLvlCache {
public:
    using Victim = std::pair<KeyT, PageT>;
    using Loader = std::function<PageT (const KeyT&)>;

private:
    using Algorithm = caches::Algorithm<PageT, KeyT>;
    std::deque<Algorithm> algorithms_;

    Loader get_slow_page_;

    //==================================================
    
public:

    struct LevelConfig {
        Policy policy_;
        std::size_t capacity_;
        std::size_t hir_size_ = 0;
    };

    //==================================================

    MultiLvlCache (std::initializer_list<LevelConfig> levels,
                   const Loader& get_slow_page);

    bool Hit (const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
MultiLvlCache<PageT, KeyT>::MultiLvlCache (
    std::initializer_list<LevelConfig> levels,
    const Loader&                      get_slow_page
) :
    get_slow_page_ (std::move (get_slow_page))
{
    if (levels.size () == 0) {
        throw std::invalid_argument ("At least one cache level is required");
    }

    if (!get_slow_page_) {
        throw std::invalid_argument ("Page loader can not be empty");
    }

    for (const auto& level : levels) {
        if (level.capacity_ == 0) {
            throw std::invalid_argument ("Cache level capacity must be positive");
        }

        algorithms_.emplace_back (
            level.policy_,
            level.capacity_,
            level.hir_size_,
            get_slow_page_
        );
    }
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool MultiLvlCache<PageT, KeyT>::Hit (const KeyT& key)
{
#if 0 
    int hit_level = -1;

    std::size_t algorithms_size = algorithms_.size ();

    for (std::size_t i = 0; i != algorithms_size; ++i) {
        Algorithm& algorithm = algorithms_[i];

        if (algorithms_[i].Contains (key)) {
            hit_level = i;
        }
    }

    if (hit_level == -1) {
        PageT page = get_slow_page_ (key);

        KeyT& victim_key = key;

        for (int i = 0; i < algorithms_.size (); i++) {
            Victim victim = algorithms_[i].Insert (victim_key, page);
            victim_key = victim.first ();
            page = victim.second ();
        }

        return false;
    }

    PageT page = algorithms_[hit_level].GetPage (key);

    KeyT victim_key = key;

    algorithms_[hit_level].Extract (key);
    for (int i; i < hit_level; i++) {
        Victim victim = algorithms_[i].Insert (key, page);
        victim_key = victim.first ();
        page = victim.second ();
    }

    return true;
#endif 
    throw std::logic_error("MultiLvlCache::Hit is not implemented");
}

//--------------------------------------------------------------------------------

} // namespace caches
