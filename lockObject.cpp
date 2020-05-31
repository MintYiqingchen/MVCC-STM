#include <chrono>

#include "lockObject.h"
#include "transaction.h"

int LockObject::read(Transaction& transaction, int& res){
	auto readSet = transaction.getReadSet();
	auto writeSet = transaction.getWriteSet();
    if(writeSet.count((void*)this) > 0) { // read local version
        res = writeSet[(void*)this].localValue;
        return 0;
    }
    if(isLocked()) { // maybe changed by other thread
        transaction.abort();
        return -1;
    }
    res = _data; // whether a lock is needed here?

    readSet[(void*)this] = {this, res};
    return 0;
}

int LockObject::write(Transaction &transaction, int res) {
	auto writeSet = transaction.getWriteSet();
    if(writeSet.count((void*)this) > 0) { // set local version
        writeSet[(void*)this].localValue = res;
        return 0;
    }

    if(isLocked()) {
        transaction.abort();
        return -1;
    }

    writeSet[(void*)this] = {this, res};
    return 0;
}

void LockObject::unlock() {
    lock_guard<mutex> lk(latch);
    if(locked) {
        objMtx.unlock();
        locked = false;
    }
}

void LockObject::commit(int value, long new_stamp) {
    // must already be locked before this function
    write_stamp = new_stamp;
    _data = value;
}

bool LockObject::isLocked() {
    lock_guard<mutex> lk(latch);
    return locked;
}

bool LockObject::validate(Transaction &t) {
    bool free = !isLocked() || isLockedBy(t.getTimestamp());
    bool pure = t.getTimestamp() <= write_stamp;
    return free && pure;
}

bool LockObject::tryLock(const long &mils, long lock_tid, LockHelper &helper) {
    lock_guard<mutex> lk(latch);
    if(!objMtx.try_lock_for(chrono::milliseconds(mils)))
        return false;
    locked = true;
    lock_stamp = lock_tid;
    helper = LockHelper(this);
    return true;
}


LockHelper::LockHelper(LockObject *obj) {
    this->obj = obj;
}

LockHelper::LockHelper(LockHelper &&other) {
    obj = other.obj;
    other.obj = nullptr;
}

LockHelper::~LockHelper() {
    if(obj != nullptr){
        obj->unlock();
    }
}

LockHelper &LockHelper::operator=(LockHelper &&other) {
    obj = other.obj;
    other.obj = nullptr;
    return *this;
}
