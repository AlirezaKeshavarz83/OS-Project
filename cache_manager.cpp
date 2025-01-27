// cache_manager.c

#include "cache_manager.h"

MemoryAccess::MemoryAccess(ll _time_stamp, ll _offset, string _type) {
    time_stamp = _time_stamp;
    offset = _offset;
    if (_type == "Write") {
        type = Write;
    } else {
        type = Read;
    }
}
