// larc.cpp

#include "larc.h"
using namespace std;

LARC_Implementation::LARC_Implementation(int capacity) : C(capacity) {
    cout << "LARC Cache Initialized with capacity: " << C << endl;
    Cr = C / 10;
}

int LARC_Implementation::access(ll offset) {
    // If in Q -> hit
    if (inQ.count(offset)) {
        Q.erase(inQ[offset]);
        Q.push_front(offset);
        inQ[offset] = Q.begin();
        Cr = max(C / 10, Cr - C / (C - Cr));
        return 0; // Hit
    }

    // Miss
    Cr = min(C - C / 10, Cr + C / max(Cr, 1));
    if (inQr.count(offset)) {
        // Promote from Qr to Q
        Qr.erase(inQr[offset]);
        inQr.erase(offset);
        Q.push_front(offset);
        inQ[offset] = Q.begin();
        if ((int)Q.size() > C) {
            ll victim = Q.back();
            Q.pop_back();
            inQ.erase(victim);
        }
    } else {
        // Insert into Qr
        Qr.push_front(offset);
        inQr[offset] = Qr.begin();
        if ((int)Qr.size() > Cr) {
            ll victim = Qr.back();
            Qr.pop_back();
            inQr.erase(victim);
        }
    }
    return 1; // Miss
}
