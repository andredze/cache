#include <cstdlib>
#include <cache.hpp>

const int kDefaultCacheSize = 10;

int main() {

	std::vector<int> data = ReadIntDataFromCin ();

	for (int i = 0; i < data.size (); i++) {
		std::cout << data.at (i) << std::endl;
	}

	return EXIT_FAILURE;
}
