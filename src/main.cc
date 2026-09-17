#include <cstdlib>
#include <string>
#include "cache.hpp"
#include "error_handle.hpp"

//————————————————————————————————————————————————————————————————————————————————

int main ()
{
	// std::deque<int> data = ReadIntDataFromCin ();

	Cache<std::string, int> cache (10);
	//TODO: Dump has not tested because i not sure about your input foo
	cache.Dump ("test1.txt");

	return EXIT_FAILURE;
}

//————————————————————————————————————————————————————————————————————————————————
