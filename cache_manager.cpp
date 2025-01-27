// cache_manager.c

#include "cache_manager.h"

MemoryAccess::MemoryAccess(ll time_stamp, ll index, string _type) : time_stamp(time_stamp), index(index) {
    if (_type == "Write") {
        type = Write;
    } else {
        type = Read;
    }
}
