#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <deque>

//--------------------------------------------------------------------------------

template <typename T, typename KeyT>
class Cache {
	std::size_t  size_ ;
	std::list<T> cache_;
	
	using ListIt = typename std::list<T>::iterator;
	std::unordered_map<int, ListIt> hash_map_;

public:
	Cache (std::size_t input_size) : size_ (input_size), cache_(input_size, 0) {}

	bool ContainsKey (KeyT key)
	{
		return hash_map_.contains (key);
	}
};

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------

// FIXME: возврат структуры ???? втф
// FIXME: почему не просто очередь, или просто массив w
std::deque<int> ReadIntDataFromCin ();
int GetCacheInputCapacity (std::deque<int>* data);
int GetNextCacheElement   (std::deque<int>* data);

//--------------------------------------------------------------------------------
