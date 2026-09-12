#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <vector>
#include <iostream>

//--------------------------------------------------------------------------------

// template <typename T>
// class Cache_t {
// 	std::size_t  size_ ;
// 	std::list<T> cache_;
	
// 	using ListIt = typename std::list<T>::iterator;	//using == typedef
// 	std::unordered_map<typename KeyT, ListIt>;

// 	Cache_t(std::size_t size);


// }; // namespace Caches

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------

std::vector<int> ReadIntDataFromCin ();

//--------------------------------------------------------------------------------