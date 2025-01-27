// nhit.h

#ifndef NHIT_H
#define NHIT_H

#include "cache_manager.h"

class N_Hit_Implementation : public CacheManager {
private:
    int cache_size, threshold;
    ll tm;
    std::map<ll, ll> count_accesses;
    std::map<ll, ll> insertion_time;
    std::set<pair<pair<ll, ll>, ll> > cache;
    std::set<ll> in_cache;

public:
    N_Hit_Implementation(int cache_size, int threshold);
    int access(ll offset) override;
};

#endif
