#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
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
	Cache(std::size_t input_capacity) : capacity_(input_capacity) {};
	
	virtual ~Cache() = default;

	size_t Size     ();
	size_t Capacity ();

	bool ContainsKey (KeyT  key );
	KeyT GenerateKey (PageT page);

	void Clear ();

	bool IsFull  ();
	bool IsEmpty ();

	void Dump (std::string file_name);
};

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
size_t Cache<PageT, KeyT>::Size ()
{
    return cache_.size ();
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
size_t Cache<PageT, KeyT>::Capacity ()
{
    return capacity_;
}

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
	// FIXME:
    return (KeyT) 1;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::IsFull ()
{
    return (cache_.size () == capacity_) ? true : false;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::IsEmpty ()
{
    return (cache_.size () == 0) ? true : false;
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

template <typename PageT, typename KeyT>
void Cache<PageT, KeyT>::Clear ()
{
    while (cache_.size ()) {
		auto victim_key = cache_.back ();

        hash_map_.erase (GenerateKey (victim_key));
            
        cache_.pop_back ();
	}

	return ;
}

//--------------------------------------------------------------------------------