#pragma once

//--------------------------------------------------------------------------------

#include <cstdlib>
#include <cstdint>
#include <queue>

//--------------------------------------------------------------------------------

using NumSequence_t = std::queue<std::size_t>;

//--------------------------------------------------------------------------------

enum class InputErr_t : std::uint8_t {
    Success,
    WrongFormat,
    ReadError
};

//--------------------------------------------------------------------------------

InputErr_t ReadNumSequence (NumSequence_t &num_sequence, std::istream input_stream);

int GetNextKey (NumSequence_t &num_sequence);

//--------------------------------------------------------------------------------