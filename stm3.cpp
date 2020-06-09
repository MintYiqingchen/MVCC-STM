//
// Created by mintyi on 6/8/20.
//
#include <thread>
#include <iostream>
#include <chrono>
#include <memory>
#include "benchmark/bitset.h"
#include "lockObject.hpp"
#include "mvccLockObject.hpp"
constexpr int ITERATION = 40000;
constexpr int SIZE = 4000;
using namespace std;
void _remove_benchmark(int start, shared_ptr<Bitset> myset) {
    for(int i = 0; i < ITERATION; ++ i) {
        myset->reset(start);
        start += 2;
        if(start >= SIZE)
            start %= SIZE;
    }
}

void _contains_benchmark(int start, shared_ptr<Bitset> myset) {
    for(int i = 0; i < ITERATION; ++ i) {
        myset->test(start);
        start += 2;
        if(start >= SIZE)
            start %= SIZE;
    }
}

void _put_benchmark(int start, shared_ptr<Bitset> myset) {
    for(int i = 0; i < ITERATION; ++ i) {
        myset->set(start);
        start += 2;
        if(start >= SIZE)
            start %= SIZE;
    }
}

void _hybrid_benchmark(int start, shared_ptr<Bitset> myset) {
    _put_benchmark(start, myset);
    _contains_benchmark(start + 1, myset);
    _remove_benchmark(start + 2, myset);
}

void hybrid_benchmark(int write_num, int read_num, shared_ptr<Bitset> myset) {
    auto start = chrono::system_clock::now();
    vector<thread> threads;
    for(int i = 0; i < write_num; ++ i){
        threads.emplace_back(_put_benchmark, i, myset);
    }
    for(int i = 0; i < read_num; ++ i) {
        threads.emplace_back(_contains_benchmark, i + write_num, myset);
    }
    for(int i = 0; i < write_num; ++ i) {
        threads.emplace_back(_remove_benchmark, i + write_num + read_num, myset);
    }
    for(int i = 0; i < threads.size(); ++ i)
        threads[i].join();
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "hybrid,"<< duration.count() << "," << (write_num*2+read_num) * ITERATION << endl;
}

int main(int argc, char** argv) {
    if(argc < 4) {
        cout << "usage: ./stm3 {base_stm|mvcc_stm} write_num read_num\n";
        cout << "example ./stm3 mvcc_stm 1 14" << endl;
        return 0;
    }
    int write_num = atoi(argv[2]);
    int read_num = atoi(argv[3]);
    if(string_view(argv[1]) == "base_stm"){
        shared_ptr<Bitset> myset(new STMBitset<Transaction, LockObject>(SIZE));
        hybrid_benchmark(write_num, read_num, myset);
        myset.reset(new STMBitset<Transaction, LockObject>(SIZE));
    }
    else if(string_view(argv[1]) == "mvcc_stm"){
        shared_ptr<Bitset> myset(new STMBitset<MVCCTransaction, MVCCLockObject>(SIZE));
        hybrid_benchmark(write_num, read_num, myset);
        myset.reset(new STMBitset<MVCCTransaction, MVCCLockObject>(SIZE));
    }
}
