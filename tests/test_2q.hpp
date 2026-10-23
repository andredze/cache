#pragma once

//--------------------------------------------------------------------------------

#include "caches/2q.hpp"
#include <gtest/gtest.h>
#include <string>

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQCacheLinkedMap, canCheckContains)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (-7);
    linked_map.PushFront (9);

    EXPECT_TRUE (linked_map.Contains (5));
    EXPECT_TRUE (linked_map.Contains (-7));
    EXPECT_TRUE (linked_map.Contains (9));

    EXPECT_FALSE (linked_map.Contains (11));
    EXPECT_FALSE (linked_map.Contains (-11));
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQCacheLinkedMap, canGetSize)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (-7);
    linked_map.PushFront (9);

    EXPECT_EQ (linked_map.GetSize (), 3);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQCacheLinkedMap, matchingPushFrontAndPopBack)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (7);
    linked_map.PushFront (9);

    EXPECT_EQ   (linked_map.PopBack (), 5);
    EXPECT_EQ   (linked_map.PopBack (), 7);

    linked_map.PushFront (11);

    EXPECT_EQ   (linked_map.PopBack (), 9);
    EXPECT_EQ   (linked_map.PopBack (), 11);

    EXPECT_EQ   (linked_map.GetSize (), 0);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQCacheLinkedMap, doesMoveToFrontIfContainsKey)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (7);
    linked_map.PushFront (9);
    linked_map.PushFront (11);

    EXPECT_TRUE (linked_map.TryMoveToFront (5));

    EXPECT_EQ (linked_map.PopBack (), 7);
    EXPECT_EQ (linked_map.PopBack (), 9);
    EXPECT_EQ (linked_map.PopBack (), 11);
    EXPECT_EQ (linked_map.PopBack (), 5);

    EXPECT_EQ (linked_map.GetSize (), 0);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQCacheLinkedMap, doesNotMoveToFrontIfNotContainsKey)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (7);
    linked_map.PushFront (9);
    linked_map.PushFront (11);

    EXPECT_FALSE (linked_map.TryMoveToFront (13));

    EXPECT_EQ (linked_map.GetSize (), 4);

    EXPECT_EQ (linked_map.PopBack (), 5);
    EXPECT_EQ (linked_map.PopBack (), 7);
    EXPECT_EQ (linked_map.PopBack (), 9);
    EXPECT_EQ (linked_map.PopBack (), 11);

    EXPECT_EQ (linked_map.GetSize (), 0);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQCache, canConstructWithRightParameters)
{
    constexpr int kCapacity = 100;

    constexpr double kPercentageSizeThresholdIn  = 27.0f;
    constexpr double kPercentageSizeThresholdOut = 53.0f;

    constexpr int kSizeThresholdIn  = kCapacity * kPercentageSizeThresholdIn  / 100;
    constexpr int kSizeThresholdOut = kCapacity * kPercentageSizeThresholdOut / 100;

    caches::TwoQCache<std::string, int> cache {
        kCapacity,
        kPercentageSizeThresholdIn,
        kPercentageSizeThresholdOut
    };

    EXPECT_EQ (cache.GetCapacity (), kCapacity);

    EXPECT_EQ (cache.GetSizeThresholdIn  (), kSizeThresholdIn);
    EXPECT_EQ (cache.GetSizeThresholdOut (), kSizeThresholdOut);
}

//————————————————————————————————————————————————————————————————————————————————

namespace caches
{

std::string IntToStringSlowGetPage (const int& key)
{
    return std::to_string (key);
}

};

//==================================================

TEST (TwoQCache, canGetPage)
{
    constexpr int kCapacity = 100;

    caches::TwoQCache<std::string, int> cache{kCapacity};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);

    EXPECT_EQ (cache.GetSize (), 0);

    EXPECT_EQ (
        cache.GetPage (10, caches::IntToStringSlowGetPage), 
        caches::IntToStringSlowGetPage(10)
    );
}

//--------------------------------------------------------------------------------

TEST (TwoQCache, canGetSize)
{
    constexpr int kCapacity = 100;

    caches::TwoQCache<std::string, int> cache{kCapacity};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);

    EXPECT_EQ (cache.GetSize (), 0);

    ASSERT_EQ (
        cache.GetPage (10, caches::IntToStringSlowGetPage), 
        caches::IntToStringSlowGetPage(10)
    );

    EXPECT_EQ (cache.GetSize (), 1);

    ASSERT_EQ (
        cache.GetPage (10, caches::IntToStringSlowGetPage), 
        caches::IntToStringSlowGetPage(10)
    );

    EXPECT_EQ (cache.GetSize (), 1);

    ASSERT_EQ (
        cache.GetPage (12, caches::IntToStringSlowGetPage), 
        caches::IntToStringSlowGetPage(12)
    );

    EXPECT_EQ (cache.GetSize (), 2);
}

//--------------------------------------------------------------------------------

TEST (TwoQCache, HitForElementInCache)
{
    constexpr int kCapacity = 100;

    caches::TwoQCache<std::string, int> cache{kCapacity};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);

    EXPECT_EQ (cache.GetSize (), 0);

    ASSERT_EQ (
        cache.GetPage (10, caches::IntToStringSlowGetPage), 
        caches::IntToStringSlowGetPage (10)
    );

    EXPECT_TRUE (cache.Hit (10, caches::IntToStringSlowGetPage));
}

//--------------------------------------------------------------------------------

TEST (TwoQCache, HitForElementNotInCache)
{
    constexpr int kCapacity = 100;

    caches::TwoQCache<std::string, int> cache{kCapacity};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);

    EXPECT_EQ (cache.GetSize (), 0);

    EXPECT_TRUE (cache.Hit (10, caches::IntToStringSlowGetPage));
}

//————————————————————————————————————————————————————————————————————————————————