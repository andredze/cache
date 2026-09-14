#include <cache.hpp>

//--------------------------------------------------------------------------------

class LRU : public Cache {
public :

//==================================================

    bool lruAdd ( int value )
    {
        bool hit = LookUpElement (value);

        if (hit) {
            cache_.remove     (value);
            cache_.push_front (value);

            return true;
        }
        else {
            cache_.pop_back   ();
            cache_.push_front (value);
        }

        return false;
    }

//==================================================
} ;