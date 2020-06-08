//
// Created by mintyi on 6/8/20.
//

#ifndef STM_HASH_SET_H
#define STM_HASH_SET_H
#include <unordered_set>
#include <mutex>
#include <functional>
#include <vector>
#include <forward_list>
#include "mvccLockObject.hpp"
#include "lockObject.hpp"
using namespace std;
#define SIZE 200
template<typename T>
class HashSet {
public:
    virtual void put(const T& key) = 0;
    virtual void remove(const T& key) = 0;
    virtual bool contains(const T& key) = 0;

};

template<typename T>
class LockHashSet: public HashSet<T> {
    std::array<std::mutex, SIZE> locks;
    std::array<forward_list<T>, SIZE> elements;
    hash<T> hf;
public:
    void put(const T& key) override {
        auto idx = hf(key) % SIZE;
        lock_guard<mutex> lk(locks[idx]);
        if(find(elements[idx].begin(), elements[idx].end(), key) != elements[idx].end())
            return;
        elements[idx].push_front(key);
    }
    bool contains(const T& key) override {
        auto idx = hf(key) % SIZE;
        lock_guard<mutex> lk(locks[idx]);
        return find(elements[idx].begin(), elements[idx].end(), key) != elements[idx].end();
    }
    void remove(const T& key) override {
        auto idx = hf(key) % SIZE;
        lock_guard<mutex> lk(locks[idx]);
        elements[idx].remove(key);
    }
};

template<typename T>
class BaseSTMHashSet: public HashSet<T> {
    std::array<, SIZE> elements;
    hash<T> hf;
public:
    void put(const T& key) override {
        auto idx = hf(key) % SIZE;
        lock_guard<mutex> lk(locks[idx]);
        if(find(elements[idx].begin(), elements[idx].end(), key) != elements[idx].end())
            return;
        elements[idx].push_front(key);
    }
    bool contains(const T& key) override {
        auto idx = hf(key) % SIZE;
        lock_guard<mutex> lk(locks[idx]);
        return find(elements[idx].begin(), elements[idx].end(), key) != elements[idx].end();
    }
    void remove(const T& key) override {
        auto idx = hf(key) % SIZE;
        lock_guard<mutex> lk(locks[idx]);
        elements[idx].remove(key);
    }
};
#endif //STM_HASH_SET_H
