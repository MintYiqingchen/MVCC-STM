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
    if(locked && lock_stamp != transaction.getTimestamp()) {
        transaction.abort();
        return -1;
    }
    res = _data; // whether a lock is needed here?
    readSet[(void*)this] = {this, _data};
    return 0;
}

int LockObject::write(Transaction &transaction, int res) {
	auto writeSet = transaction.getWriteSet();
    if(writeSet.count((void*)this) > 0) { // set local version
        writeSet[(void*)this].localValue = res;
        return 0;
    }
    if(locked && lock_stamp != transaction.getTimestamp()) {
        transaction.abort();
        return -1;
    }
    writeSet[(void*)this] = {this, res};
    return 0;
}

template<typename T>
bool LockObject::tryLock(T time, Transaction& t) {
    if(mtx.try_lock_for(time)){
        locked = true;
        lock_stamp = t.getTimestamp();
        return true;
    }
    return false;
}

void LockObject::unlock() {
    locked = false;
    mtx.unlock();
}

