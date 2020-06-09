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

void worker1() {
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

constexpr int THREAD_NUM = 32;
MVCCLockObject sVec1{std::vector<char>(THREAD_NUM, 0)};
MVCCLockObject sVec2{std::vector<char>(THREAD_NUM, 0)};
atomic_uint success{0};
atomic_uint fail{0};
void readTransaction() {
    MVCCTransaction t;
    vector<char> local;
    if(sVec1.read(t, local) != 0) {
        fail++;
        return;
    }
    if(sVec2.read(t, local) != 0){
        fail++;
        return;
    }
    this_thread::sleep_for(chrono::milliseconds(200));
    if(t.commit()){
        success++;
        return;
    }
    fail++;
}
void writeTransaction1(int i) {
    MVCCTransaction t;
    vector<char> local;
    if(sVec1.read(t, local) != 0) {
        fail++;
        return;
    }
    int pos = i % local.size();
    local[pos] = i;
    sVec2.write(t, local);
    if(t.commit()){
        success++;
        return;
    }
    fail++;
}
int main(int argc, char** argv) {
    vector<thread> threads;
    for(int i = 0; i < THREAD_NUM; ++ i) {
        if(i % 2 == 0)
            threads.emplace_back(writeTransaction1, i);
        threads.emplace_back(readTransaction);

    }

    for(thread& t: threads)
        t.join();
    cout << "finish" << endl;
    cout << success.load() << " " << fail.load() << endl;
}

