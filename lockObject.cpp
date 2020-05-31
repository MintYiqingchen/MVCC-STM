#include <chrono>

#include "lockObject.h"
#include "transaction.h"

int LockObject::read(Transaction& transaction, int& res){
	auto readSet = transaction.getReadSet();
	auto writeSet = transaction.getWriteSet();
    if(write_stamp > transaction.getTimestamp()) {
        transaction.abort();
        return -1;
    }
    if(writeSet.count((void*)this) > 0) { // read local version
        res = writeSet[(void*)this].localValue;
        if(read_stamp < transaction.getTimestamp()) {read_stamp = transaction.getTimestamp();}
        return 0;
    }
    if(locked && lock_stamp != transaction.getTimestamp()) {
        transaction.abort();
        return -1;
    }
    res = _data; // whether a lock is needed here?
    readSet[(void*)this] = {this, _data};
    if(read_stamp < transaction.getTimestamp()) {read_stamp = transaction.getTimestamp();}
    return 0;
}

int LockObject::write(Transaction &transaction, int res) {
	auto writeSet = transaction.getWriteSet();
    if(write_stamp > transaction.getTimestamp() || read_stamp > transaction.getTimestamp()) {
        transaction.abort();
        return -1;
    }
    if(writeSet.count((void*)this) > 0) { // set local version
        writeSet[(void*)this].localValue = res;
        write_stamp = transaction.getTimestamp();
        return 0;
    }
    if(locked && lock_stamp != transaction.getTimestamp()) {
        transaction.abort();
        return -1;
    }
    writeSet[(void*)this] = {this, res};
    write_stamp = transaction.getTimestamp();
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


bool LockObject::validate(Transaction &transaction){
    Transaction::Status status = transaction.getStatus();
    if(status == Transaction::Status::COMMITED) {return true;}
    if(status == Transaction::Status::ACTIVE){
        bool free = !locked || (lock_stamp == transaction.getTimestamp());
        bool pure = read_stamp <= transaction.getTimestamp();
        return free && pure;
    }
    return false;
}
