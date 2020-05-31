#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_
#include <atomic>
#include <memory>
#include <unordered_map>

#include "LockObject.h"
using namespace std;

class LockObject;


struct Pack {
    LockObject* ptr;
    int localValue;
};
class Transaction{
    long start_stamp;
    unordered_map<void*, Pack> writeSet, readSet; // object address -> the local value

public:
    enum Status {ABORTED,ACTIVE,COMMITED};
    static atomic_long GLOBAL_CLOCK;

	Transaction();
    ~Transaction();
	Status getStatus();
	bool commit();
	bool abort();
    unordered_map<void*, Pack>& getReadSet() {return readSet; };
    unordered_map<void*, Pack>& getWriteSet() {return writeSet; };
    long getTimestamp() const {return start_stamp; };

private:
    Status status;
};

#endif