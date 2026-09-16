#include <cstdlib>
#include <string>
#include "cache.hpp"
#include "error_handle.hpp"

//————————————————————————————————————————————————————————————————————————————————

int main ()
{
	// std::deque<int> data = ReadIntDataFromCin ();

	Cache<std::string, int> cache (10);

	PrintError ("testim");

	return EXIT_FAILURE;
}

//————————————————————————————————————————————————————————————————————————————————
