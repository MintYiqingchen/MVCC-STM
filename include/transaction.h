#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_
#include <atomic>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <any>
#include <vector>
#include "Lockable.h"
using namespace std;

class Transaction{
    struct Pack {
        Lockable* ptr;
        any localValue;
    };
    long start_stamp, commit_stamp{-1};
    unordered_map<void*, Pack> writeSet, readSet; // object address -> the local value

public:
    enum Status {ABORTED,ACTIVE,COMMITTED};
    static atomic_long GLOBAL_CLOCK;

	Transaction();
    ~Transaction();
	Status getStatus();
	bool commit();
	bool abort();
    unordered_map<void*, Pack>& getReadSet() {return readSet; };
    unordered_map<void*, Pack>& getWriteSet() {return writeSet; };
    long getTimestamp() const {return start_stamp; };
    long getCommitStamp() const {return commit_stamp;}
private:
    Status status;
};



#endif