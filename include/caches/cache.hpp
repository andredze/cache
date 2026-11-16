#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <deque>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <variant>

#include "arc.hpp"
#include "lru.hpp"
#include "lfu.hpp"
#include "lirs.hpp"
#include "2q.hpp"

//--------------------------------------------------------------------------------

namespace caches {

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class MultiLvlCache {

public:

    using Victim = std::pair<KeyT, PageT>;
    using Loader = std::function<PageT (const KeyT&)>;

private:

    //==================================================

    using TwoQLevel = caches::TwoQ<PageT, KeyT, Loader>;

    using Algorithm = std::variant<
        caches::ARC <PageT, KeyT>,
        caches::LRU <PageT, KeyT>,
        caches::LFU <PageT, KeyT>,
        caches::LIRS<PageT, KeyT>,
        TwoQLevel
    >;

    //==================================================

    std::deque<Algorithm> algorithms_;

    //==================================================

public:

    //==================================================

    enum class Policy {
        LRU, LFU, LIRS, ARC, TwoQ
    };

    //==================================================

    struct LevelConfig {
        Policy policy_;
        std::size_t capacity;
        std::size_t hir_size = 0;
    };

    //==================================================

    MultiLvlCache (std::initializer_list<LevelConfig> levels, const Loader& get_slow_page)
    {
        for (const auto& level : levels) {
            if (level.capacity == 0) {
                throw std::invalid_argument ("Cache level capacity must be positive");
            }

            switch (level.policy_) {
                case Policy::LRU:
                    algorithms_.emplace_back (
                        std::in_place_type<caches::LRU<PageT, KeyT>>,
                        level.capacity, get_slow_page
                    );
                    break;
                case Policy::LFU:
                    algorithms_.emplace_back (
                        std::in_place_type<caches::LFU<PageT, KeyT>>,
                        level.capacity, get_slow_page
                    );
                    break;
                case Policy::ARC:
                    algorithms_.emplace_back (
                        std::in_place_type<caches::ARC<PageT, KeyT>>,
                        level.capacity, get_slow_page
                    );
                    break;
                case Policy::LIRS:
                    if (level.hir_size == 0 || level.hir_size >= level.capacity) {
                        throw std::invalid_argument ("LIRS requires 0 < HIR size < capacity");
                    }
                    algorithms_.emplace_back (
                        std::in_place_type<caches::LIRS<PageT, KeyT>>,
                        level.capacity - level.hir_size, level.hir_size, get_slow_page
                    );
                    break;
                case Policy::TwoQ:
                    algorithms_.emplace_back (
                        std::in_place_type<TwoQLevel>,
                        level.capacity, get_slow_page
                    );
                    break;
            }
        }
    }

    ~MultiLvlCache() = default;

    bool Hit (KeyT key);

};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool MultiLvlCache<PageT, KeyT>::Hit (const KeyT& key)
{
    int hit_level = -1;

    for (int i = 0; i < algorithms_.size (); i++) {
        if (algorithms_[i].contain (key)) {
            hit_level = i;
        }
    }

    if (hit_level == -1) {
        PageT page = GetSlowPage (key);
        int victim_key = key;
        for (int i; i < algorithms_.size (); i++) {
            Victim victim = algorithms_[i].insert (victim_key, page);
            victim_key = victim.first ();
            page = victim.second ();
        }

        return false;
    }

    PageT page = algorithms_[hit_level].GetPage (key);
    int victim_key = key;
    algorithms_[hit_level].Extract (key);
    for (int i; i < hit_level; i++) {
        Victim victim = algorithms_[i].Insert (key, page);
        victim_key = victim.first ();
        page = victim.second ();
    }

    return true;
}

//--------------------------------------------------------------------------------

} // namespace caches
