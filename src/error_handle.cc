#include "error_handle.hpp"
#include <source_location>
#include <iostream>

//————————————————————————————————————————————————————————————————————————————————

void PrintError (const std::string_view      message,
                 const std::source_location& location)
{
    std::cerr << "[ERROR] At: "
              << location.file_name ()     << ":"
              << location.line ()          << ":" 
              << location.column ()        << ": " 
              << location.function_name () << "   "
              << message                   << std::endl;

    // for variable args length (in progress)
    // ((std::cerr << std::forward<Args>(args) << " "), ...);
}

//————————————————————————————————————————————————————————————————————————————————