#include <cstdlib>
#include <cache.hpp>

const int kDefaultCacheSize = 10;

int main() {
	Caches::Cache_t<std::string> (kDefaultCacheSize);

	return EXIT_FAILURE;
}
