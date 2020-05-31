#ifndef _LOCKOBJECT_H_
#define _LOCKOBJECT_H_
#include <atomic>
#include <mutex>

#include "transaction.h"

using namespace std;
struct LockHelper;

class LockObject{
public:
	explicit LockObject(int data):_data(data) {};

	int read(Transaction&, int&);

	int write(Transaction&, int);

	void commit(int value, long new_stamp);
    void unlock();

    bool tryLock(const long& time, long lock_tid, LockHelper&);

    bool isLockedBy(long tid) const {return lock_stamp == tid;}
    bool isLocked();
    bool validate(Transaction&);
protected:
    bool locked = false;
	long write_stamp{-1}, lock_stamp{-1};
	timed_mutex objMtx;
	mutex latch; // short period lock, must acquired when change isLocked
	atomic_int _data;
	friend struct LockHelper;
};

struct LockHelper {
    LockObject* obj;
    LockHelper():obj(nullptr){};
    explicit LockHelper(LockObject* obj);
    LockHelper(LockHelper&&) noexcept ;
    LockHelper& operator = (LockHelper&&) noexcept ;
    ~LockHelper();
};
#endif