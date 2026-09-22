#include <iostream>
#include "tests.hpp"
#include "input.hpp"
#include "error_handle.hpp"
#include "lru.hpp"

//————————————————————————————————————————————————————————————————————————————————

template <typename KeyT>
static std::string slow_get_string_page (KeyT key)
{
	volatile int tmp = 0;

	for (std::size_t i = 0; i < 10'000'000; i++) {
		tmp = tmp + 1;
	}

	return "page" + std::to_string(key);
}

//————————————————————————————————————————————————————————————————————————————————

int Test1LruCache ()
{
	std::ifstream test_data_file (kTest1FileName);

	if (!test_data_file.is_open()) {
		PrintError ("Failed opening test file");
        return 1;
    }

	//==================================================

	NumSequence num_seq;

	num_seq.Read (test_data_file);

	num_seq.Print ();
	
	int cache_size = num_seq.GetNextNum ();

	if (cache_size == -1) {
		return 1;
	}
	
	int data_size = num_seq.GetNextNum ();

	if (data_size == -1) {
		return 1;
	}

	LRUCache<std::string, int> lru_cache (cache_size);

	if (lru_cache.LogFileOpen (kLogFileName) != CacheErr_t::kSuccess) {
		return -1;
	}

	//==================================================

	int hits_count = 0;

	for (int i = 0; i < data_size; i++) {
		int cur_key = num_seq.GetNextNum ();

		if (cur_key == -1) {
			return 1;
		}

		bool hit = lru_cache.Add (cur_key, slow_get_string_page);
	
		if (hit) {
			hits_count++;
		}

		if (lru_cache.LogDump () != CacheErr_t::kSuccess) {
			return -1;
		};
	}

	//==================================================
	
	std::cout << "hit_count = " << hits_count << std::endl;

	lru_cache.LogFileClose ();

	return 0;
}

//————————————————————————————————————————————————————————————————————————————————
