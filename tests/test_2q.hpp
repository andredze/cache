#pragma once

//--------------------------------------------------------------------------------

#include "caches/2q.hpp"
#include <gtest/gtest.h>
#include <string>

//————————————————————————————————————————————————————————————————————————————————

namespace caches
{

std::string IntToStringSlowGetPage (const int& key)
{
    return std::to_string (key);
}

};

//==================================================

TEST (TwoQLinkedMap, canCheckContains)
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

TEST (TwoQLinkedMap, canGetSize)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (-7);
    linked_map.PushFront (9);

    EXPECT_EQ (linked_map.GetSize (), 3);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQLinkedMap, matchingPushFrontAndPopBack)
{
    caches::LinkedMap<int> linked_map{};

    linked_map.PushFront (5);
    linked_map.PushFront (7);
    linked_map.PushFront (9);

    EXPECT_EQ (linked_map.PopBack (), 5);
    EXPECT_EQ (linked_map.PopBack (), 7);

    linked_map.PushFront (11);

    EXPECT_EQ (linked_map.PopBack (), 9);
    EXPECT_EQ (linked_map.PopBack (), 11);

    EXPECT_EQ (linked_map.GetSize (), 0);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQLinkedMap, doesMoveToFrontIfContainsKey)
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

TEST (TwoQLinkedMap, doesNotMoveToFrontIfNotContainsKey)
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

TEST (TwoQ, canConstructWithRightParameters)
{
    constexpr int kCapacity = 100;

    constexpr std::size_t kPercentageIn  = 27;
    constexpr std::size_t kPercentageOut = 53;

    constexpr int kSizeThresholdIn  = kCapacity * kPercentageIn  / 100;
    constexpr int kSizeThresholdOut = kCapacity * kPercentageOut / 100;

    caches::TwoQ<std::string, int> cache {
        kCapacity,
        caches::IntToStringSlowGetPage,
        kPercentageIn,
        kPercentageOut
    };

    EXPECT_EQ (cache.GetCapacity (), kCapacity);

    EXPECT_EQ (cache.GetSizeThresholdIn  (), kSizeThresholdIn);
    EXPECT_EQ (cache.GetSizeThresholdOut (), kSizeThresholdOut);
}

//————————————————————————————————————————————————————————————————————————————————

TEST (TwoQ, canGetPage)
{
    constexpr int kCapacity = 100;

    caches::TwoQ<std::string, int> cache {kCapacity, caches::IntToStringSlowGetPage};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);
    EXPECT_EQ (cache.GetSize (), 0);

    EXPECT_EQ (cache.GetPage (10), caches::IntToStringSlowGetPage(10));
}

//--------------------------------------------------------------------------------

TEST (TwoQ, canGetSize)
{
    constexpr int kCapacity = 100;

    caches::TwoQ<std::string, int> cache {kCapacity, caches::IntToStringSlowGetPage};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);
    EXPECT_EQ (cache.GetSize (), 0);
    ASSERT_EQ (cache.GetPage (10), caches::IntToStringSlowGetPage (10));
    EXPECT_EQ (cache.GetSize (), 1);
    ASSERT_EQ (cache.GetPage (10), caches::IntToStringSlowGetPage (10));
    EXPECT_EQ (cache.GetSize (), 1);
    ASSERT_EQ (cache.GetPage (12), caches::IntToStringSlowGetPage (12));
    EXPECT_EQ (cache.GetSize (), 2);
}

//--------------------------------------------------------------------------------

TEST (TwoQ, HitForElementInCache)
{
    constexpr int kCapacity = 100;

    caches::TwoQ<std::string, int> cache {kCapacity, caches::IntToStringSlowGetPage};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);
    EXPECT_EQ (cache.GetSize (), 0);
    ASSERT_EQ (cache.GetPage (10), caches::IntToStringSlowGetPage (10));

    EXPECT_TRUE (cache.Hit (10));
}

//--------------------------------------------------------------------------------

TEST (TwoQ, HitForElementNotInCache)
{
    constexpr int kCapacity = 100;

    caches::TwoQ<std::string, int> cache {kCapacity, caches::IntToStringSlowGetPage};

    EXPECT_EQ (cache.GetCapacity (), kCapacity);
    EXPECT_EQ (cache.GetSize (), 0);

    EXPECT_FALSE (cache.Hit (10));
}

//————————————————————————————————————————————————————————————————————————————————
