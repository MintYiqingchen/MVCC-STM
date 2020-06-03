#ifndef _LOCKOBJECT_H_
#define _LOCKOBJECT_H_
#include <atomic>
#include <mutex>

#include "mvccTransaction.h"
#include "LockableList.h"
using namespace std;

class MVCCTransaction;

template<typename T>
struct Version: public LockableNode {
    T _data;
    explicit Version(T x, long rstamp = -1, long wstamp = -1):_data(x){
        _read_stamp = rstamp;
        _write_stamp = wstamp;
    }
};

template<typename T>
class MVCCLockObject: public LockableList {
    Version<T>* latest;
public:
	explicit MVCCLockObject(T data){ // initialize first version
	    Version<T>* v = new Version<T>(data);
	    insert_after(head, v);
	    latest = v;
	}

	int read(MVCCTransaction& transaction, T& res) {
	    // read is always satisfied
        // first read from write set
        auto &writeSet = transaction.getWriteSet();
        if(writeSet.count((void*)this) > 0) { // read local version
            res = any_cast<T>(writeSet[(void*)this].localValue);
            return 0;
        }
        Version<T>* v = (Version<T>*)(predecessor(transaction.getTimestamp()));
        // get lock
        {
            lock_guard<timed_mutex> lk(v->_mtx);
            res = v->_data;
            v->updateRstamp(transaction.getTimestamp());
        }
        return 0;
	}

	int write(MVCCTransaction& transaction, T val) {
        auto& writeSet = transaction.getWriteSet();
        if(writeSet.count((void*)this) > 0) { // set local version
            writeSet[(void*)this].localValue = make_any<T>(val);
            return 0;
        }
        // create a new Pack
        writeSet[(void*)this] = {this, make_any<T>(val)};
        return 0;
	}

	bool commit(const any& val, long tstamp, LockableNode* pred) override {
        // must be called after get pred locked
        if(!pred->validate(tstamp)) return false;
        T x = any_cast<T>(val);
        Version<T>* node = new Version<T>(x, tstamp, tstamp);
        if(pred == latest)
            latest = node;
        insert_after(pred, node);
        return true;
	}

	T getValue() const {
	    lock_guard<timed_mutex> lk(latest->_mtx);
        return latest->_data;
	}

	long getStamp() const {
        lock_guard<timed_mutex> lk(latest->_mtx);
        return latest->getWstamp();
	}
};
#endif