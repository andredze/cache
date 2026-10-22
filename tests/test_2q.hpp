#pragma once

//--------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <string>
#include "2q.hpp"

//————————————————————————————————————————————————————————————————————————————————

TEST(TwoQCacheLinkedMap, Constructor)
{
    caches::LinkedMap<int> linked_map {};

    linked_map.PushFront (5);

    EXPECT_TRUE (linked_map.Contains (5));

    EXPECT_EQ (linked_map.GetSize (), 1);

    EXPECT_EQ (linked_map.PopBack (), 5);

    EXPECT_EQ (linked_map.GetSize (), 0);
}

//————————————————————————————————————————————————————————————————————————————————

TEST(TwoQCache, Constructor)
{
    constexpr int kCapacity = 100;

    caches::TwoQCache<std::string, int> cache {kCapacity};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);
}

//————————————————————————————————————————————————————————————————————————————————
