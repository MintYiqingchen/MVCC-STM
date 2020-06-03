//
// Created by mintyi on 6/2/20.
//

#include "Lockable.h"

void Lockable::unlock() {
    lock_guard<mutex> lk(latch);
    if(locked) {
        objMtx.unlock();
        locked = false;
    }
}

bool Lockable::tryLock(const long &mils, long lock_tid, LockHelper &helper) {
    lock_guard<mutex> lk(latch);
    if(!objMtx.try_lock_for(chrono::milliseconds(mils)))
        return false;
    locked = true;
    lock_stamp = lock_tid;
    helper = LockHelper(this);
    return true;
}

bool Lockable::isLockedBy(long tid) const {return lock_stamp == tid;}

bool Lockable::isLocked(){
    lock_guard<mutex> lk(latch);
    return locked;
}
