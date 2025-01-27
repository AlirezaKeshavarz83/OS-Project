// cache_manager.h

#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

struct MemoryAccess {
    ll time_stamp;
    ll index;
    enum AccessType {
        Read,
        Write
    } type;

    MemoryAccess(ll _time_stamp, ll _index, string _type);
};

class CacheManager {
public:
    virtual int access(ll index) = 0; // this function simulates an access request to our cache
                                      // return 0 if it's a hit and 1 otherwise
    virtual ~CacheManager() {}
};

#endif
