// nhit.cpp

#include "nhit.h"
using namespace std;

N_Hit_Implementation::N_Hit_Implementation(int cache_size, int threshold) : cache_size(cache_size), threshold(threshold), tm(0) {
    cout << "N-Hit Cache Initialized with capacity: " << cache_size << " and threshold: " << threshold << endl;
}

int N_Hit_Implementation::access(ll index) {
    // check if the page is found in cache
    if(in_cache.find(index) != in_cache.end()){
        // update our sorted list of pages in cache
        cache.erase({{count_accesses[index], insertion_time[index]}, index});
        count_accesses[index]++; // increase the number of accesses
        cache.insert({{count_accesses[index], insertion_time[index]}, index});
        return 0; // Hit
    }
    count_accesses[index]++; // increase the number of accesses
    if(count_accesses[index] < threshold){
        return 1;
    }
    // insert the page in cache
    insertion_time[index] = ++tm; // set and increase the timestamp
    in_cache.insert(index);
    cache.insert({{count_accesses[index], insertion_time[index]}, index});
    // Evict the minimum accessed page. (the currently added page may be evicted right away)
    if((int)cache.size() > cache_size){
        ll victim = cache.begin()->second;
        cache.erase(cache.begin());
        in_cache.erase(victim);
    }
    
    return 1; // Miss
}
