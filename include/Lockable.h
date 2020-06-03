//
// Created by mintyi on 6/2/20.
//

#ifndef STM_LOCKABLE_H
#define STM_LOCKABLE_H

#include <mutex>
#include <any>
using namespace std;

struct LockHelper;
class Transaction;

class Lockable {
protected:
    bool locked = false;
    long lock_stamp{-1};
    mutex latch;
    timed_mutex objMtx;
    friend struct LockHelper;
public:
    void unlock();
    bool tryLock(const long& mils, long lock_tid, LockHelper& helper);
    bool isLockedBy(long tid) const;
    bool isLocked();
    virtual void commit(const any& value, long new_stamp) = 0;
    virtual bool validate(Transaction& t) = 0;
};

struct LockHelper {
    Lockable* obj;
    LockHelper():obj(nullptr){};
    explicit LockHelper(Lockable* obj) {
        this->obj = obj;
    }
    LockHelper(LockHelper&& other) noexcept {
        obj = other.obj;
        other.obj = nullptr;
    }
    LockHelper& operator = (LockHelper&& other) noexcept {
        obj = other.obj;
        other.obj = nullptr;
        return *this;
    }
    ~LockHelper() {
        if(obj != nullptr){
            obj->unlock();
        }
    }
};



#endif //STM_LOCKABLE_H
