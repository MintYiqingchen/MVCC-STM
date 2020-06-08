#ifndef _MVCCTRANSACTION_H_
#define _MVCCTRANSACTION_H_
#include <atomic>
#include <memory>
#include <map>
#include <unordered_map>
#include "LockableList.h"
#include "TxManager.h"
#include <any>
#include <set>
using namespace std;

struct Pack {
    LockableList* ptr;
    any localValue;
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