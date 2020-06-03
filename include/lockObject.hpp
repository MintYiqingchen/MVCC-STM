#ifndef _LOCKOBJECT_H_
#define _LOCKOBJECT_H_
#include <atomic>
#include <mutex>
#include "transaction.h"
#include "Lockable.h"
using namespace std;


template<typename T>
class LockObject: public Lockable {
public:
	explicit LockObject(T data):_data(data) {};

	int read(Transaction& transaction, T& res) {
        auto& readSet = transaction.getReadSet();
        auto& writeSet = transaction.getWriteSet();
        if(writeSet.count((void*)this) > 0) { // read local version
            res = any_cast<T>(writeSet[(void*)this].localValue);
            return 0;
        }
        if(isLocked()) { // maybe changed by other thread
            transaction.abort();
            return -1;
        }
        {
            lock_guard<mutex> lk(data_latch);
            res = _data;
        }
        readSet[(void*)this] = {this, make_any<T>(res)};
        return 0;
	}

	int write(Transaction& transaction, const T& res) {
        auto& writeSet = transaction.getWriteSet();
        if(writeSet.count((void*)this) > 0) { // set local version
            writeSet[(void*)this].localValue = res;
            return 0;
        }

        if(isLocked()) {
            transaction.abort();
            return -1;
        }

        writeSet[(void*)this] = {this, make_any<T>(res)};
        return 0;
	}

	void commit(const any& value, long new_stamp) override {
        // must already be locked before this function
        write_stamp = new_stamp;
        _data = any_cast<T>(value);
	}

    bool validate(Transaction& t) override {
        bool free = !isLocked() || isLockedBy(t.getTimestamp());
        bool pure = t.getTimestamp() > write_stamp;
        return free && pure;
    }

    T getValue() const {return _data;}
    long getStamp() const {return write_stamp;}
protected:

	long write_stamp{-1};
	mutex data_latch; // short period lock, must acquired when change isLocked
	T _data;

};


#endif