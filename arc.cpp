// arc.cpp

#include "arc.h"
using namespace std;

ARC_Implementation::ARC_Implementation(int capacity) 
    : totalCapacity(capacity), p(0)
{
    cout << "ARC Cache Initialized with capacity: " << capacity << endl;
}

void ARC_Implementation::replace(int tt) {
    // We are gonna pop T1/T2 and move the page to B1/B2
    // the selection is gonna be based on the pointer p that indicated the dedicated capacity of T1
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
    // tt==1 -> a page is going to be pushed into T1 after this operation
    // tt==2 -> a page is going to be pushed into T2 after this operation
    if (tt == 1) {
        // the total space occupied by our lists will increase only when an element is being pushed into T1
        // We have to evict a page if we are at the limit.
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

int ARC_Implementation::access(ll index) {
    // Case 1: Hit in T1
    if (inT1.count(index)) {
        T1.erase(inT1[index]);
        inT1.erase(index);
        T2.push_front(index);
        inT2[index] = T2.begin();
        return 0; // Hit
    }
    // Case 2: Hit in T2
    else if (inT2.count(index)) {
        T2.erase(inT2[index]);
        T2.push_front(index);
        inT2[index] = T2.begin();
        return 0; // Hit
    }
    // Case 3: Miss but page is found in B1
    else if (inB1.count(index)) {
        // Increase T1 capacity cause a page was hit in B1
        int delta = max((int)B2.size() / max((int)B1.size(), 1), 1);
        p = min(p + delta, totalCapacity);
        // Make space for the new page about to be added to T2
        replace(2);
        B1.erase(inB1[index]);
        inB1.erase(index);
        // Move the page to T2
        T2.push_front(index);
        inT2[index] = T2.begin();
        return 1; // Miss
    }
    // Case 3: Miss but in B2
    else if (inB2.count(index)) {
        // Increase T2 capacity cause a page was hit in B2
        int delta = max((int)B1.size() / max((int)B2.size(), 1), 1);
        p = max(p - delta, 0);
        // Make space for the new page about to be added to T2
        replace(2);
        B2.erase(inB2[index]);
        inB2.erase(index);
        // Move the page to T2
        T2.push_front(index);
        inT2[index] = T2.begin();
        return 1; // Miss
    }
    // Case 4: Miss and not in any list
    else {
        // Evict one page if the limit is met.
        if ((int)(T1.size() + T2.size()) >= totalCapacity) {
            replace(1);
        }
        // Add the page to T1.
        T1.push_front(index);
        inT1[index] = T1.begin();
        return 1; // Miss
    }
}
