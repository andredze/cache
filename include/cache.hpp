#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <deque>
#include <string>
#include <iterator>

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class Cache {
protected:
	//TODO: we need an exception if we have size = 0
	std::size_t      size_;
	std::list<PageT> cache_;
	
	using ListIt = typename std::list<PageT>::iterator;
	std::unordered_map<KeyT, ListIt> hash_map_;

public:
	Cache (std::size_t input_size) : size_ (input_size), cache_(input_size, PageT{}) {}

	bool ContainsKey (KeyT  key );
	KeyT GenerateKey (PageT page);

	void Dump (std::string file_name);
};

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------
