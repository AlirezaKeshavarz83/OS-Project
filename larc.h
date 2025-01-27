// larc.h

#ifndef LARC_H
#define LARC_H

#include "cache_manager.h"

class LARC_Implementation : public CacheManager {
private:
    int C;
    int Cr;
    std::list<ll> Q, Qr;
    std::unordered_map<ll, std::list<ll>::iterator> inQ, inQr;

public:
    LARC_Implementation(int capacity);
    int access(ll index) override;
};

#endif
