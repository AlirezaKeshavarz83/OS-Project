// lru.cpp

#include "lru.h"

LRU_Implementation::LRU_Implementation(int cache_size) : cache_size(cache_size) {}

int LRU_Implementation::access(ll offset) {
    // Cache hit
    if (cacheMap.find(offset) != cacheMap.end()) {
        lruList.erase(cacheMap[offset]);
        lruList.push_front(offset);
        cacheMap[offset] = lruList.begin();
        return 0; // Hit
    }

    // Cache miss
    if ((int)lruList.size() == cache_size) {
        ll lru = lruList.back();
        lruList.pop_back();
        cacheMap.erase(lru);
    }

    lruList.push_front(offset);
    cacheMap[offset] = lruList.begin();
    return 1; // Miss
}
