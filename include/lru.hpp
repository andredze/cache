#include <cache.hpp>

//--------------------------------------------------------------------------------

template <typename T, typename KeyT>
class LRU : public Cache<T, KeyT> {
public :
	bool Add(int value);
};
