#include "input.hpp"
#include "error_handle.hpp"
#include <queue>
#include <iostream>

//————————————————————————————————————————————————————————————————————————————————

InputErr_t ReadNumSequence (NumSequence_t &num_sequence, std::istream input_stream)
{
    unsigned int cur_number = 0;

    while (input_stream >> cur_number) {
        num_sequence.push (cur_number);
    }

    if (input_stream.eof ()) {
        return InputErr_t::Success;
    }

    if (input_stream.fail ()) {
        PrintError ("Wrong input format, expected sequence of non-negative numbers");
        return InputErr_t::WrongFormat;
    }
 
    if (input_stream.bad ()) {
        PrintError ("Read error occurred");
        return InputErr_t::ReadError;
    }

    return InputErr_t::Success;
}

//————————————————————————————————————————————————————————————————————————————————

int GetNextKey (NumSequence_t &num_sequence)
{
    if (num_sequence.size () == 0) {
        return -1;
    }

    std::size_t next_num = num_sequence.front ();

    num_sequence.pop ();

    return next_num;
}

//————————————————————————————————————————————————————————————————————————————————