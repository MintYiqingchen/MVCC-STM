//
// Created by mintyi on 5/30/20.
//
#include <thread>
#include "mvccTransaction.h"
#include "mvccLockObject.hpp"
#include <vector>
#include <iostream>
using namespace std;
MVCCLockObject A(200), B(250), C(100);
constexpr int CHANGE = 200;
mutex printMtx;
void report_status(MVCCTransaction& t) {
    lock_guard<mutex> lk(printMtx);
    cout << this_thread::get_id() << " report:";
    cout << " status: " << t.getStatus() << " ts "<<t.getTimestamp();
    cout << " A: "<< A.getValue() << " ts "<<A.getStamp();
    cout << " B: " << B.getValue() << " ts "<<B.getStamp();
    cout << " C: " << C.getValue() << " ts "<<C.getStamp() << endl;
}

void worker() {
    MVCCTransaction transaction; // start a transaction
    int a, b, c;
    // a -> b -> c -> a
    if (A.read(transaction, a) != 0 || B.read(transaction, b) != 0) {
        transaction.abort();
        goto final;
    }
    if (a >= CHANGE) {
        if (B.write(transaction, b + CHANGE) || A.write(transaction, a - CHANGE)) {
            transaction.abort();
            goto final;
        }
    } else {
        transaction.abort();
        goto final;
    }
    if (C.read(transaction, c) != 0 || B.read(transaction, b) != 0) {
        transaction.abort();
        goto final;
    }
    if (b >= CHANGE) {
        if (B.write(transaction, b - CHANGE) || C.write(transaction, c + CHANGE)){
            transaction.abort();
            goto final;
        }
    } else {
        transaction.abort();
        goto final;
    }
    if (A.read(transaction, a) != 0 || C.read(transaction, c) != 0) {
        transaction.abort();
        goto final;
    }
    if (c >= CHANGE) {
        if (A.write(transaction, a + CHANGE) || C.write(transaction, c - CHANGE)){
            transaction.abort();
            goto final;
        }
    } else {
        transaction.abort();
        goto final;
    }
    transaction.commit();

final:
    report_status(transaction);

}
int main(int argc, char** argv) {
    vector<thread> threads;
    for(int i = 0; i < 10; ++ i) {
        threads.emplace_back(worker);
    }
    for(thread& t: threads)
        t.join();
    cout << "finish" << endl;
}

