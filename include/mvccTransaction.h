#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_
#include <atomic>
#include <memory>
#include <map>
#include <unordered_map>
#include "LockableList.h"
#include <any>
using namespace std;

struct Pack {
    LockableList* ptr;
    any localValue;
};
class GCManager {
    static long min_active_stamp; //  = INT64_MAX;
    static unordered_map<void*, LockableList*> objects;
};
class MVCCTransaction{
    long start_stamp; // indicate serialization order
    // object address -> the local value
    // ordered by address -> avoid deadlock
    map<void*, Pack> writeSet;
public:
    enum Status {ABORTED,ACTIVE,COMMITTED};
    static atomic_long GLOBAL_CLOCK;

	MVCCTransaction();
    ~MVCCTransaction();
	Status getStatus();
	bool commit();
	bool abort();
    map<void*, Pack>& getWriteSet() {return writeSet; };
    long getTimestamp() const {return start_stamp; };
private:
    Status status;
};
#endif