// nhit.cpp

#include "nhit.h"
using namespace std;

N_Hit_Implementation::N_Hit_Implementation(int cache_size, int threshold) : cache_size(cache_size), threshold(threshold), tm(0) {
    cout << "N-Hit Cache Initialized with threshold: " << threshold << endl;
}

int N_Hit_Implementation::access(ll offset) {
    if(in_cache.find(offset) != in_cache.end()){
        cache.erase({{count_accesses[offset], insertion_time[offset]}, offset});
        count_accesses[offset]++;
        cache.insert({{count_accesses[offset], insertion_time[offset]}, offset});
        return 0; // Hit
    }
    count_accesses[offset]++;
    if(count_accesses[offset] < threshold){
        return 1;
    }
    insertion_time[offset] = ++tm;
    in_cache.insert(offset);
    cache.insert({{count_accesses[offset], insertion_time[offset]}, offset});
    if((int)cache.size() > cache_size){
        ll victim = cache.begin()->second;
        cache.erase(cache.begin());
        in_cache.erase(victim);
    }
    
    return 1; // Miss
}
