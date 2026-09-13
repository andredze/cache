#include <cstdlib>
#include <cache.hpp>

const int kDefaultCacheSize = 10;

int main() {

	std::deque<int> data = ReadIntDataFromCin ();

	while (data.size () > 0) {
		std::cout << GetNextCacheElement (&data) << std::endl;
	}

	return EXIT_FAILURE;
}
