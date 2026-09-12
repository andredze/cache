#include <cache.hpp>

//————————————————————————————————————————————————————————————————————————————————

std::vector<int> ReadIntDataFromCin ()
{
    std::vector<int> input_data = {};

    int cur_number = 0;

    while (std::cin >> cur_number) {
        input_data.push_back (cur_number);
    }

    return input_data;
}

//————————————————————————————————————————————————————————————————————————————————