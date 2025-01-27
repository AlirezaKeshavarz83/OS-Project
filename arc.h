// arc.h

#ifndef ARC_H
#define ARC_H

#include "cache_manager.h"

class ARC_Implementation : public CacheManager {
private:
    int totalCapacity;
    int p; // Adaptive parameter

    // Lists to store pages
    std::list<ll> T1, T2; 
    std::list<ll> B1, B2; 

    // Maps to store iterators for quick lookup
    std::unordered_map<ll, std::list<ll>::iterator> inT1, inT2;
    std::unordered_map<ll, std::list<ll>::iterator> inB1, inB2;

    void replace(int tt);

public:
    ARC_Implementation(int capacity);
    int access(ll index) override;
};

#endif
