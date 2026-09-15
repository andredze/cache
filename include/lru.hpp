#include <cache.hpp>

//--------------------------------------------------------------------------------

template <typename PageT, typename KeyT>
class LRU : public Cache<PageT, KeyT> {
public :
	bool Add (int value);
};
