#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <deque>
#include <iostream>

//--------------------------------------------------------------------------------

const int kPoison = 66677752;

//--------------------------------------------------------------------------------

class Cache {
public:
	std::size_t    size_ ;
	std::list<int> cache_;
	
	using ListIt = typename std::list<int>::iterator;	//using == typedef
	std::unordered_map<int, ListIt> hash_map_;

	//==================================================

	Cache (std::size_t input_size) : size_ (input_size) 
	{
		for (int i = 0; i < input_size; i++) {
			cache_.insert(cache_.end(), kPoison);
		}
	} // class creator

	//==================================================

	// int LookUpElement (int target_value)
	// {
	// 	if ()
	// }

	//==================================================
}; // namespace Caches

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------

std::deque<int> ReadIntDataFromCin ();
int GetCacheInputCapacity (std::deque<int>* data);
int GetNextCacheElement   (std::deque<int>* data);

//--------------------------------------------------------------------------------