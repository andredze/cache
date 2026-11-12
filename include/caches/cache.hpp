#pragma once

//--------------------------------------------------------------------------------

#include <variant>
#include <vector>

#include "input.hpp"

#include "arc.hpp"
#include "lru.hpp"
#include "lfu.hpp"
#include "lirs.hpp"
#include "2q.hpp"

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class Cache {

private:

    //==================================================

    using Algorithm = std::variant<
        caches::ARC <PageT, KeyT>,
        caches::LRU <PageT, KeyT>,
        caches::LFU <PageT, KeyT>,
        caches::LIRS<PageT, KeyT>,
        caches::TwoQ<PageT, KeyT>
    >;

    //==================================================

    std::vector<Algorithm> algorithms_;

    //==================================================

public:

    //==================================================

    template <typename AlgorithmT, typename... Args>
    void AddAlgorithm (Args&&... args);

    bool Hit (const KeyT& key);
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
template <typename AlgorithmT, typename... Args>
void Cache<PageT, KeyT>::AddAlgorithm (Args&&... args)
{
    algorithms_.emplace_back (std::in_place_type<AlgorithmT>, std::forward<Args> (args)...);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::Hit (const KeyT& key)
{
    for (auto& algorithm : algorithms_) {
        bool hit = std::visit ([&](auto& cache) { return cache.Hit (key); }, algorithm);

        if (hit) {
            return true;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------
