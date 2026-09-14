#include <cache.hpp>
#include <iostream>

//————————————————————————————————————————————————————————————————————————————————

//deque = double-ended queue

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

int GetNextCacheElement (std::deque<int>* data)
{
    if ((*data).size () == 0) { //check size
        return kPoison;
    }

    //==================================================

    int cap = (*data).front (); //take data

    (*data).pop_front ();       //remove first element

    return cap;
}

//————————————————————————————————————————————————————————————————————————————————

int GetCacheInputCapacity (std::deque<int>* data)
{
    return GetNextCacheElement (data);
}

//————————————————————————————————————————————————————————————————————————————————
