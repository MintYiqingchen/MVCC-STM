//
// Created by mintyi on 6/3/20.
//

#ifndef STM_LOCKABLELIST_H
#define STM_LOCKABLELIST_H
#include <mutex>
#include <any>
class LockableNode {
public:
    void updateRstamp(long s);
    long getWstamp() const{ return _write_stamp;};
    long getRstamp() const{ return _read_stamp;};
    bool validate(long tstamp);
    std::timed_mutex _mtx;
    std::mutex latch;
    LockableNode* next = nullptr;
    bool tryLock(long mils, long tstamp, std::unique_lock<std::timed_mutex>&);
protected:
    long _write_stamp{-1};
    long _read_stamp{-1};
};
class LockableList { // an optimistic linked list
protected:
    LockableNode* const head = new LockableNode(); // dummy head
    LockableNode* const tail = new LockableNode(); // dummy tail
public:
    LockableList() { head->next = tail; }
    // find the version of Node whose write stamp is largest one less than or equal to tstamp
    LockableNode* predecessor(long tstamp);

    // insert newNode after predecessor
    int insert_after(LockableNode* predecessor, LockableNode* newNode);
    ~LockableList();

    // commit a new version to the list
    virtual bool commit(const std::any&, long tstamp, LockableNode* pred) = 0;
};

#endif
