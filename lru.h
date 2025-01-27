// lru.h

#ifndef LRU_H
#define LRU_H

#include "cache_manager.h"

class LRU_Implementation : public CacheManager {
private:
    int cache_size;
    std::list<ll> lru_list;
    std::unordered_map<ll, std::list<ll>::iterator> in_cache;

public:
    LRU_Implementation(int cache_size);
    int access(ll index) override;
};

#endif
