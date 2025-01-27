// lru.cpp

#include "lru.h"

LRU_Implementation::LRU_Implementation(int cache_size) : cache_size(cache_size) {
    cout << "LRU Cache Initialized with capacity: " << cache_size << endl;
}

int LRU_Implementation::access(ll index) {
    // Cache hit
    if (in_cache.find(index) != in_cache.end()) {
        lru_list.erase(in_cache[index]);
        lru_list.push_front(index);
        in_cache[index] = lru_list.begin();
        return 0; // Hit
    }

    // Cache miss
    if ((int)lru_list.size() == cache_size) {
        ll lru = lru_list.back();
        lru_list.pop_back();
        in_cache.erase(lru);
    }

    lru_list.push_front(index);
    in_cache[index] = lru_list.begin();
    return 1; // Miss
}
