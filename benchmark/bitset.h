//
// Created by mintyi on 6/8/20.
//

#ifndef STM_BITSET_H
#define STM_BITSET_H
#include <mutex>
#include <memory>
#include <vector>
using namespace std;
class Bitset {
public:
    virtual void set(unsigned i) = 0;
    virtual bool test(unsigned i) = 0;
    virtual void reset(unsigned i) = 0;
    virtual bool trySet(unsigned i) {
        set(i);
        return true;
    }
    virtual bool tryTest(unsigned i, bool& res) {
        res = test(i);
        return true;
    }
    virtual bool tryReset(unsigned i) {
        reset(i);
        return true;
    }
};

class SimpleBitset: public Bitset {
    vector<char> _vec;
    vector<unique_ptr<mutex>> lks;
public:
    SimpleBitset(size_t size) {
        auto s = size / (sizeof(char) * 8) + 1;
        _vec.resize(s, 0);
        lks.reserve(s);
        for(auto i = 0; i < s; ++ i){
            lks.emplace_back(new mutex);
        }
    }

    void set(unsigned i) override {
        auto idx = i / (sizeof(char) * 8);
        auto posi = i % (sizeof(char) * 8);
        lock_guard<mutex> lk(*lks[idx]);
        _vec[idx] |= (1 << posi);
    }

    bool test(unsigned i) override {
        auto idx = i / (sizeof(char) * 8);
        auto posi = i % (sizeof(char) * 8);
        lock_guard<mutex> lk(*lks[idx]);
        return (_vec[idx] & (1<<posi)) != 0;
    }

    void reset(unsigned i) override {
        auto idx = i / (sizeof(char) * 8);
        auto posi = i % (sizeof(char) * 8);
        lock_guard<mutex> lk(*lks[idx]);
        _vec[idx] &= ~(1 << posi);
    }
};

template<typename TX, template<typename> typename TO>
class STMBitset: public Bitset {
    vector<shared_ptr<TO<char>>> _vec;
    long backoff;
public:
    STMBitset(std::size_t size, long backoff = 100) {
        this->backoff = backoff;
        auto s = size / (sizeof(char) * 8) + 1;
        _vec.reserve(s);
        for(auto i = 0; i < s; ++ i){
            _vec.emplace_back(new TO<char>(0));
        }
    }
    bool trySet(unsigned i) override {
        auto idx = i / (sizeof(char) * 8);
        auto posi = i % (sizeof(char) * 8);
        char c = 0;
        TX t;
        if(_vec[idx]->read(t, c) != 0) {
            return false;
        }
        c |= (1 << posi);
        if(_vec[idx]->write(t, c) != 0) {
            return false;
        }
        return t.commit();
    }
    void set(unsigned i) override {
        while(true) {
            if(trySet(i)) break;
            this_thread::sleep_for(chrono::microseconds(backoff));
        }
    }

    bool tryTest(unsigned i, bool &res) {
        auto idx = i / (sizeof(char) * 8);
        auto posi = i % (sizeof(char) * 8);
        char c;
        TX t;
        if(_vec[idx]->read(t, c) != 0) {
            return false;
        }
        if(!t.commit()) {
            return false;
        }
        res = (c & (1<<posi)) != 0;
        return true;
    }
    bool test(unsigned i) override {
        bool res = false;
        while(true) {
            if(tryTest(i, res)) break;
            this_thread::sleep_for(chrono::microseconds(backoff));
        }
        return res;
    }
    bool tryReset(unsigned i) override {
        auto idx = i / (sizeof(char) * 8);
        auto posi = i % (sizeof(char) * 8);
        char c;
        TX t;
        if(_vec[idx]->read(t, c) != 0) {
            return false;
        }
        c &= ~(1 << posi);
        if(_vec[idx]->write(t, c) != 0) {
            return false;
        }
        return t.commit();
    }
    void reset(unsigned i) override {
        while(true) {
            if(tryReset(i)) break;
            this_thread::sleep_for(chrono::microseconds(backoff));
        }
    }

};
#endif //STM_BITSET_H
