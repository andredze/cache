#include "cache.hpp"
#include <queue>
#include <iostream>

//————————————————————————————————————————————————————————————————————————————————

int ParseInputData (std::string              input_buffer,
                    std::queue<std::size_t> &cache_sizes,
                    std::queue<std::size_t> &keys)
{
    return 0;
}
// //deque = double-ended queue

std::deque<int> ReadIntDataFromCin ()
{
    std::deque<int> input_data = {};

    int cur_number = 0;

    while (std::cin >> cur_number) {
        input_data.push_back (cur_number);
    }

    return input_data;
}

//————————————————————————————————————————————————————————————————————————————————

int GetNextKey (std::deque<int>* data)
{
    if ((*data).size () == 0) {
        return -1;
    }

    //==================================================

    int cap = (*data).front (); //take data

    (*data).pop_front ();       //remove first element

    return cap;
}

//————————————————————————————————————————————————————————————————————————————————