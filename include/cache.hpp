#pragma once

//--------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <list>
#include <unordered_map>
#include <string>
#include <iterator>
#include <fstream>
#include "error_handle.hpp"

//--------------------------------------------------------------------------------

enum class CacheErr_t : std::uint8_t {
	kSuccess     = 0,
	kOpenFileErr = 1,
};

//--------------------------------------------------------------------------------

std::string kLogFileName = "dump.log";

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class Cache {
	struct Node {
		PageT page;
		KeyT  key;
	};

	std::size_t capacity_;

	std::ofstream log_file_;

protected:
	//TODO: we need an exception if we have size = 0
	std::list<Node> cache_;
	
	using ListIt = typename std::list<Node>::iterator;
	std::unordered_map<KeyT, ListIt> hash_map_;

public:
	Cache(std::size_t input_capacity) : capacity_(input_capacity) {};
	
	virtual ~Cache() = default;

	size_t Size     ();
	size_t Capacity ();

	bool ContainsKey (KeyT key);
	void Clear ();
	bool IsFull ();
	bool IsEmpty ();

	CacheErr_t LogFileOpen (std::string log_file_name);
	CacheErr_t LogDump ();
	void       LogFileClose ();
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
bool Cache<PageT, KeyT>::IsFull ()
{
    return (cache_.size () == capacity_) ? true : false;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
bool Cache<PageT, KeyT>::IsEmpty ()
{
    return (cache_.size () == 0);
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
CacheErr_t Cache<PageT, KeyT>::LogFileOpen (std::string log_file_name)
{
    log_file_.open (log_file_name);
    
    if (!log_file_.is_open ()) {
		PrintError ("Failed to open log file");
        return CacheErr_t::kOpenFileErr;
    }

	return CacheErr_t::kSuccess;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
CacheErr_t Cache<PageT, KeyT>::LogDump ()
{
    if (!log_file_.is_open ()) {
		PrintError ("Can not dump in log file, open it first");
        return CacheErr_t::kOpenFileErr;
    }

    std::size_t i = 0;

	log_file_ << "-----------------------------------------" << std::endl;
	log_file_ << "Dumping cache list:" << std::endl;
	log_file_ << "cache_.size = " << cache_.size () << std::endl;

    for (auto it = cache_.begin (); it != cache_.end (); it++, i++) {
        log_file_ << i << "[" << it->key << "] = " << it->page << std::endl;
    }

	return CacheErr_t::kSuccess;
}

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
void Cache<PageT, KeyT>::LogFileClose ()
{
    if (!log_file_.is_open ()) {
        return;
    }

    log_file_.close ();
}

//--------------------------------------------------------------------------------


template <typename PageT, typename KeyT>
void Cache<PageT, KeyT>::Clear ()
{
    while (cache_.size ()) {
		auto victim_key = cache_.back ().key;

        hash_map_.erase (victim_key);
            
        cache_.pop_back ();
	}

	return;
}

//--------------------------------------------------------------------------------