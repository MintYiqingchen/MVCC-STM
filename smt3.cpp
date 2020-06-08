//
// Created by mintyi on 6/8/20.
//
#include <thread>
#include <iostream>
#include "hashset/hash_set.h"
#include <chrono>
#include <memory>
#define ITERATION 40000
using namespace std;
void _remove_benchmark(int start, shared_ptr<HashSet<int>> myset) {
    for(int i = 0; i < ITERATION; ++ i) {
        myset->remove(start);
        start += 2;
    }
}
void remove_benchmark(int thread_num, shared_ptr<HashSet<int>> myset) {
    vector<thread> threads;
    while(thread_num -- ) {
        threads.emplace_back(_remove_benchmark, thread_num + 2, myset);
    }
    for(int i = 0; i < threads.size(); ++ i)
        threads[i].join();
}
void _contains_benchmark(int start, shared_ptr<HashSet<int>> myset) {
    for(int i = 0; i < ITERATION; ++ i) {
        myset->contains(start);
        start += 2;
    }
}
void contains_benchmark(int thread_num, shared_ptr<HashSet<int>> myset) {
    vector<thread> threads;
    while(thread_num -- ) {
        threads.emplace_back(_contains_benchmark, thread_num + 1, myset);
    }
    for(int i = 0; i < threads.size(); ++ i)
        threads[i].join();
}
void _put_benchmark(int start, shared_ptr<HashSet<int>> myset) {
    for(int i = 0; i < ITERATION; ++ i) {
        myset->put(start);
        start += 2;
    }
}
void put_benchmark(int thread_num, shared_ptr<HashSet<int>> myset) {
    vector<thread> threads;
    while(thread_num -- ) {
        threads.emplace_back(_put_benchmark, thread_num, myset);
    }
    for(int i = 0; i < threads.size(); ++ i)
        threads[i].join();
}
void seperate_benchmark(int thread_num, shared_ptr<HashSet<int>> myset) {
    auto start = chrono::system_clock::now();
    put_benchmark(thread_num, myset);
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "put,"<< duration.count() << "," << thread_num * ITERATION << endl;

    start = chrono::system_clock::now();
    contains_benchmark(thread_num, myset);
    end = chrono::system_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "contains,"<< duration.count() << "," << thread_num * ITERATION << endl;

    start = chrono::system_clock::now();
    remove_benchmark(thread_num, myset);
    end = chrono::system_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "remove,"<< duration.count() << "," << thread_num * ITERATION << endl;
}
void _hybrid_benchmark(int start, shared_ptr<HashSet<int>> myset) {
    _put_benchmark(start, myset);
    _contains_benchmark(start + 1, myset);
    _remove_benchmark(start + 2, myset);
}
void hybrid_benchmark(int thread_num, shared_ptr<HashSet<int>> myset) {
    auto start = chrono::system_clock::now();
    vector<thread> threads;
    for(int i = 0; i < thread_num; ++ i){
        threads.emplace_back(_hybrid_benchmark, thread_num, myset);
    }
    for(int i = 0; i < threads.size(); ++ i)
        threads[i].join();
    auto end = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "hybrid,"<< duration.count() << "," << thread_num * ITERATION << endl;
}
int main(int argc, char** argv) {
    if(argc < 3) {
        cout << "usage: ./stm3 {lock_coarse} thread_num\n";
        cout << "example ./stm3 seperate 64" << endl;
    }
    int thread_num = atoi(argv[2]);
    if(string_view(argv[1]) == "lock_coarse"){
        shared_ptr<HashSet<int>> myset(new LockHashSet<int>);
        seperate_benchmark(thread_num, myset);
        myset.reset(new LockHashSet<int>);
        hybrid_benchmark(thread_num, myset);
    }
    else {

    }
}
