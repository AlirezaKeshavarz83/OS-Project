// nhit.h

#ifndef NHIT_H
#define NHIT_H

#include "cache_manager.h"

class N_Hit_Implementation : public CacheManager {
private:
    int cache_size, threshold;
    ll tm;                                   // current timestamp
    std::map<ll, ll> count_accesses;         // the number of times each page has been accessed
    std::map<ll, ll> insertion_time;         // the timestamp each page was added to the cache
    std::set<pair<pair<ll, ll>, ll> > cache; // pages in cache sorted by (count_accesses, insertion_time)
    std::set<ll> in_cache;                   // the set of pages currently stored in the cache

public:
    N_Hit_Implementation(int cache_size, int threshold);
    int access(ll index) override;
};

#endif
