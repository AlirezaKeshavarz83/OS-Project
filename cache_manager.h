// cache_manager.h

#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

struct MemoryAccess {
    ll time_stamp;
    ll offset;
    enum AccessType {
        Read,
        Write
    } type;

    MemoryAccess(ll _time_stamp, ll _offset, string _type);
};

class CacheManager {
public:
    virtual int access(ll offset) = 0;
    virtual ~CacheManager() {}
};

#endif
