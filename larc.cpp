// larc.cpp

#include "larc.h"
using namespace std;

LARC_Implementation::LARC_Implementation(int capacity) : C(capacity) {
    cout << "LARC Cache Initialized with capacity: " << C << endl;
    Cr = C / 10;
}

int LARC_Implementation::access(ll index) {
    // this one implemented solely based on the pseudocode provided in the papar
    // If in Q -> hit
    if (inQ.count(index)) {
        Q.erase(inQ[index]);
        Q.push_front(index);
        inQ[index] = Q.begin();
        Cr = max(C / 10, Cr - C / (C - Cr)); // update Qr's capacity
        return 0; // Hit
    }

    // Miss
    Cr = min(C - C / 10, Cr + C / max(Cr, 1));
    if (inQr.count(index)) {
        // Promote from Qr to Q
        Qr.erase(inQr[index]);
        inQr.erase(index);
        Q.push_front(index);
        inQ[index] = Q.begin();
        // evict an element from Q if necessary
        if ((int)Q.size() > C) {
            ll victim = Q.back();
            Q.pop_back();
            inQ.erase(victim);
        }
    } else {
        // Insert into Qr
        Qr.push_front(index);
        inQr[index] = Qr.begin();
        // evict an element from Qr if necessary
        if ((int)Qr.size() > Cr) {
            ll victim = Qr.back();
            Qr.pop_back();
            inQr.erase(victim);
        }
    }
    return 1; // Miss
}
