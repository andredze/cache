#pragma once

//--------------------------------------------------------------------------------

#include <cstdlib>
#include <cstdint>
#include <queue>
#include <istream>

//--------------------------------------------------------------------------------

enum class InputErr_t : std::uint8_t {
    Success,
    WrongFormat,
    ReadError
};

//--------------------------------------------------------------------------------

class NumSequence {
    std::queue<std::size_t> queue_;

public:
    InputErr_t Read (std::istream& input_stream);

    int GetNextNum ();

    std::size_t GetSize ();

    void Print ();
};

//--------------------------------------------------------------------------------