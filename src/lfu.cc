#include "lfu.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace lfu {

//————————————————————————————————————————————————————————————————————————————————

void Test ()
{
    size_t passed = 0;
    size_t total  = 0;

    //--------------------------------------------------------------------------
    // Test 1: Access, обычная последовательность

    {
        constexpr int keys[] = {
            1, 2, 3, 1, 2, 4, 1, 2, 3, 4
        };

        constexpr size_t expected_hits = 4;

        LFU<int, int> cache(3);

        size_t hits = 0;

        for (const int key : keys) {
            hits += cache.Access(key);
        }

        total++;

        if (hits == expected_hits) {
            passed++;
            std::cout << "Access test 1: PASSED\n";
        }
        else {
            std::cout << "Access test 1: FAILED\n";
            std::cout << "Expected hits: " << expected_hits << '\n';
            std::cout << "Actual hits:   " << hits << '\n';
        }
    }

    //--------------------------------------------------------------------------
    // Test 2: Access, capacity = 1

    {
        constexpr int keys[] = {
            5, 5, 5, 5
        };

        constexpr size_t expected_hits = 3;

        LFU<int, int> cache(1);

        size_t hits = 0;

        for (const int key : keys) {
            hits += cache.Access(key);
        }

        total++;

        if (hits == expected_hits) {
            passed++;
            std::cout << "Access test 2: PASSED\n";
        }
        else {
            std::cout << "Access test 2: FAILED\n";
            std::cout << "Expected hits: " << expected_hits << '\n';
            std::cout << "Actual hits:   " << hits << '\n';
        }
    }

    //--------------------------------------------------------------------------
    // Test 3: проверка вытеснения LFU

    {
        constexpr int keys[] = {
            1, 2, 1, 3, 1, 2
        };

        constexpr size_t expected_hits = 2;

        LFU<int, int> cache(2);

        size_t hits = 0;

        for (const int key : keys) {
            hits += cache.Access(key);
        }

        total++;

        if (hits == expected_hits) {
            passed++;
            std::cout << "Access test 3: PASSED\n";
        }
        else {
            std::cout << "Access test 3: FAILED\n";
            std::cout << "Expected hits: " << expected_hits << '\n';
            std::cout << "Actual hits:   " << hits << '\n';
        }
    }

    //--------------------------------------------------------------------------
    // Test 4: GetPage два раза для одного ключа

    {
        LFU<int, int> cache(3);

        const int key = 10;

        int page1 = cache.GetPage(key);
        int page2 = cache.GetPage(key);

        total++;

        if (page1 == page2) {
            passed++;
            std::cout << "GetPage test 1: PASSED\n";
        }
        else {
            std::cout << "GetPage test 1: FAILED\n";
            std::cout << "First page:  " << page1 << '\n';
            std::cout << "Second page: " << page2 << '\n';
        }
    }

    //--------------------------------------------------------------------------
    // Test 5: GetPage + проверка, что страница действительно попала в кеш

    {
        LFU<int, int> cache(3);

        const int key = 20;

        int page = cache.GetPage(key);

        total++;

        if (cache.Contains(key)) {
            passed++;
            std::cout << "GetPage test 2: PASSED\n";
        }
        else {
            std::cout << "GetPage test 2: FAILED\n";
            std::cout << "Page was not added to cache\n";
        }
    }

    //--------------------------------------------------------------------------

    std::cout << "\n------------------------\n";
    std::cout << "Passed: " << passed << " / " << total << '\n';

    if (passed == total) {
        std::cout << "ALL TESTS PASSED\n";
    }
    else {
        std::cout << "SOME TESTS FAILED\n";
    }
}

//————————————————————————————————————————————————————————————————————————————————

}