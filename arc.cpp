// arc.cpp

#include "arc.h"
using namespace std;

ARC_Implementation::ARC_Implementation(int capacity) 
    : totalCapacity(capacity), p(0)
{
    cout << "ARC Cache Initialized with capacity: " << capacity << endl;
}

void ARC_Implementation::replace(int tt) {
    int o = 0;
    if ((int)T1.size() > p) {
        o = 1;
    } else if ((int)T1.size() < p) {
        o = 2;
    } else {
        if (tt == 1) {
            if (T1.empty()) o = 2;
            else o = 1;
        } else {
            if (T2.empty()) o = 1;
            else o = 2;
        }
    }

    if (o == 1) {
        // Move LRU page from T1 to B1
        ll victim = T1.back();
        T1.pop_back();
        inT1.erase(victim);
        B1.push_front(victim);
        inB1[victim] = B1.begin();
    } else {
        // Move LRU page from T2 to B2
        ll victim = T2.back();
        T2.pop_back();
        inT2.erase(victim);
        B2.push_front(victim);
        inB2[victim] = B2.begin();
    }

    if (tt == 1) {
        if ((int)(T1.size() + T2.size() + B1.size() + B2.size()) >= 2 * totalCapacity) {
            if (o == 1) {
                ll victim = B1.back();
                B1.pop_back();
                inB1.erase(victim);
            } else {
                ll victim = B2.back();
                B2.pop_back();
                inB2.erase(victim);
            }
        }
    }
}

int ARC_Implementation::access(ll offset) {
    // Case 1: Hit in T1
    if (inT1.count(offset)) {
        T1.erase(inT1[offset]);
        inT1.erase(offset);
        T2.push_front(offset);
        inT2[offset] = T2.begin();
        return 0; // Hit
    }
    // Case 1: Hit in T2
    else if (inT2.count(offset)) {
        T2.erase(inT2[offset]);
        T2.push_front(offset);
        inT2[offset] = T2.begin();
        return 0; // Hit
    }
    // Case 2: Miss but in B1
    else if (inB1.count(offset)) {
        int delta = max((int)B2.size() / max((int)B1.size(), 1), 1);
        p = min(p + delta, totalCapacity);
        replace(2);
        B1.erase(inB1[offset]);
        inB1.erase(offset);
        T2.push_front(offset);
        inT2[offset] = T2.begin();
        return 1; // Miss
    }
    // Case 3: Miss but in B2
    else if (inB2.count(offset)) {
        int delta = max((int)B1.size() / max((int)B2.size(), 1), 1);
        p = max(p - delta, 0);
        replace(2);
        B2.erase(inB2[offset]);
        inB2.erase(offset);
        T2.push_front(offset);
        inT2[offset] = T2.begin();
        return 1; // Miss
    }
    // Case 4: Miss and not in any list
    else {
        if ((int)(T1.size() + T2.size()) >= totalCapacity) {
            replace(1);
        }
        T1.push_front(offset);
        inT1[offset] = T1.begin();
        return 1; // Miss
    }
}
