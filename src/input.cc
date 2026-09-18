#include "input.hpp"
#include "error_handle.hpp"
#include <queue>
#include <iostream>

//————————————————————————————————————————————————————————————————————————————————

InputErr_t NumSequence::Read (std::istream& input_stream)
{
    unsigned int cur_number = 0;

    while (input_stream >> cur_number) {
        queue_.push (cur_number);
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

int NumSequence::GetNextNum ()
{
    if (queue_.size () == 0) {
        return -1;
    }

    std::size_t next_num = queue_.front ();

    queue_.pop ();

    return next_num;
}

//————————————————————————————————————————————————————————————————————————————————

std::size_t NumSequence::GetSize ()
{
    return queue_.size ();
}

//————————————————————————————————————————————————————————————————————————————————

void NumSequence::Print ()
{
    std::size_t seq_size = GetSize ();

    std::queue<std::size_t> copy = queue_;

    std::cout << "Printing num sequence:" << std::endl;

    for (std::size_t i = 0; i < seq_size; i++) {
        std::cout << copy.front () << " ";
        copy.pop (); // removes first element
    }

    std::cout << std::endl;
}

//————————————————————————————————————————————————————————————————————————————————