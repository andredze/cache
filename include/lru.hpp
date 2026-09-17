    #pragma once
    #include "cache.hpp"

    //--------------------------------------------------------------------------------

    template <typename PageT, typename KeyT>
    class LRUCache : public Cache<PageT, KeyT> {
    public:
        using Cache<PageT, KeyT>::cache_;
        using Cache<PageT, KeyT>::hash_map_;


        using Cache<PageT, KeyT>::Cache      ;
        using Cache<PageT, KeyT>::Size       ;
        using Cache<PageT, KeyT>::Capacity   ;
        using Cache<PageT, KeyT>::ContainsKey;
        using Cache<PageT, KeyT>::GenerateKey;
        using Cache<PageT, KeyT>::Clear      ;
        using Cache<PageT, KeyT>::IsFull     ;
        using Cache<PageT, KeyT>::IsEmpty    ;
        using Cache<PageT, KeyT>::Dump       ;

        bool Add (KeyT key, PageT (*slow_get_page) (KeyT));
    };

    //--------------------------------------------------------------------------------

    template <typename PageT, typename KeyT>
    bool LRUCache<PageT, KeyT>::Add (KeyT key, PageT (*slow_get_page) (KeyT))
    {
        bool hit = ContainsKey (key);

        if (hit) {
            auto node_it = hash_map_.at (key);

            cache_.splice (cache_.begin (), cache_, node_it);

            return true;
        }
        else {
            PageT page = slow_get_page (key); 

            if (IsFull ()) {
                auto victim_key = cache_.back ();

                hash_map_.erase (GenerateKey (victim_key));
                
                cache_.pop_back ();
            }


            cache_.push_front (page);
            hash_map_[key] = cache_.begin ();

        }

        return false;
    };

    //--------------------------------------------------------------------------------