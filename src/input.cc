#include "cache.hpp"
#include "error_handle.hpp"
#include <queue>
#include <iostream>

using NumSequence_t = std::queue<std::size_t>;

//————————————————————————————————————————————————————————————————————————————————

int ReadNumSequence (NumSequence_t &num_sequence, std::istream input_stream)
{
    int cur_number = 0;

    while (input_stream >> cur_number) {
        if (cur_number < 0) {
            // TODO: change PrintError, so you can pass multiple args
            PrintError("Expected non-negative number, given: ");
            return -1;
        }

        num_sequence.push (cur_number);
    }

    return 0;
}

//————————————————————————————————————————————————————————————————————————————————

int GetNextKey (NumSequence_t &num_sequence)
{
    if (num_sequence.size () == 0) {
        return -1;
    }

    //==================================================

    std::size_t next_num = num_sequence.front ();

    num_sequence.pop ();

    return next_num;
}

//————————————————————————————————————————————————————————————————————————————————