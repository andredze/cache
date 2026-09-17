#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <deque>
#include <string>
#include <iterator>
#include <fstream>

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class Cache {
	std::size_t capacity_;

protected:
	//TODO: we need an exception if we have size = 0
	std::list<PageT> cache_;
	
	using ListIt = typename std::list<PageT>::iterator;
	std::unordered_map<KeyT, ListIt> hash_map_;

public:
	Cache (std::size_t input_capacity) : capacity_ (input_capacity), cache_(input_capacity, PageT{}) {}

	bool ContainsKey (KeyT  key );
	KeyT GenerateKey (PageT page);

	bool IsFull ();

	void Dump (std::string file_name);
};

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::ContainsKey (KeyT key)
{
    return hash_map_.contains (key);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
KeyT Cache<PageT, KeyT>::GenerateKey (PageT page)
{
    return (KeyT) page;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::IsFull ()
{
    return (cache_.size () == capacity_) ? true : false;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void Cache<PageT, KeyT>::Dump (std::string file_name)
{
    std::ofstream file;
    file.open (file_name);
    
    if (!file.is_open ()) {
        return;
    }

    std::size_t i = 0;
    for (auto it = cache_.begin (); it != cache_.end (); it++, i++) {
        file << "elem " << i << " = " << *it << "\n";
    }
}

//--------------------------------------------------------------------------------