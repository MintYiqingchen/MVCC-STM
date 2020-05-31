//
// Created by mintyi on 5/30/20.
//
#include <thread>
#include "transaction.h"
#include "lockObject.h"
#include <vector>
#include <iostream>
using namespace std;
LockObject A{500}, B{500}, C{500};
constexpr int CHANGE = 200;
void report_status(Transaction& t) {
    // TODO print transaction, A,B and C status
}

void worker() {
    Transaction transaction; // start a transaction
    int a, b, c;
    // a -> b -> c -> a
    if(A.read(transaction, a) != 0 || B.read(transaction, b) != 0) {
        goto final;
    }
    if(a >= CHANGE){
        if(B.write(transaction, b + CHANGE) || A.write(transaction, a - CHANGE))
            goto final;
    } else {
        transaction.abort();
        goto final;
    }

    if(C.read(transaction, c) != 0 || B.read(transaction, b) != 0) {
        goto final;
    }
    if(b >= CHANGE){
        if(B.write(transaction, b - CHANGE) || C.write(transaction, c + CHANGE))
            goto final;
    } else {
        transaction.abort();
        goto final;
    }

    if(A.read(transaction, a) != 0 || C.read(transaction, c) != 0) {
        goto final;
    }
    if(c >= CHANGE){
        if(A.write(transaction, a + CHANGE) || C.write(transaction, c - CHANGE))
            goto final;
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
    for(int i = 0; i < 4; ++ i) {
        threads.emplace_back(worker);
    }
    for(thread& t: threads)
        t.join();
    cout << "finish" << endl;
}

