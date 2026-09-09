#pragma once

#include <cstddef>
#include <list>
#include <unordered_map>

namespace Caches {

template <typename T>
class Cache_t {
	std::size_t size_;
	std::list<T> cache_;
	
	using ListIt = typename std::list<T>::iterator;
	std::unordered_map<typename KeyT, ListIt>;

	Cache_t(std::size_t size);


}; // namespace Caches
