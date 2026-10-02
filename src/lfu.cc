#include "lfu.hpp"

//————————————————————————————————————————————————————————————————————————————————

namespace lfu {

//————————————————————————————————————————————————————————————————————————————————

void Test()
{
    constexpr int data[] = {
        1, 2, 3, 1, 2, 4, 1, 2, 3, 4
    };

    constexpr std::size_t capacity = 3;

    lfu::LFU<int, int> cache(capacity);

    std::size_t hits = 0;

    for (const int key : data) {
        if (cache.GetPage(key)) {
            hits++;
        }
    }

    std::cout << "Hits: " << hits << '\n';
}

//————————————————————————————————————————————————————————————————————————————————

}