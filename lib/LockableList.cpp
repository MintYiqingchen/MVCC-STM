//
// Created by mintyi on 6/3/20.
//

#include "LockableList.h"
using namespace std;

bool LockableNode::tryLock(long mils, long tstamp, unique_lock<timed_mutex> &lk) {
    lk = unique_lock<timed_mutex>(_mtx, defer_lock);
    if(!lk.try_lock_for(chrono::milliseconds(mils)))
        return false;
    return true;
}

bool LockableNode::validate(long tstamp) {
    // must be validate after acquire lock
    return _read_stamp <= tstamp;
}

void LockableNode::updateRstamp(long s) {
    lock_guard<mutex> lk(latch);
    if(_read_stamp < s)
        _read_stamp = s;
}

LockableList::~LockableList() {
    LockableNode* curr = head;
    LockableNode* next;
    while(curr != nullptr){
        next = curr->next;
        delete curr;
        curr = next;
    }
}

LockableNode *LockableList::predecessor(long tstamp) {
    LockableNode* curr = head;
    while(curr->next != tail){
        if(curr->next->getWstamp() <= tstamp)
            curr = curr->next;
        else break;
    }
    return curr;
}

int LockableList::insert_after(LockableNode *pre, LockableNode *newNode) {
    // must be invoked after acquire pre's wlock and pre->next's wlock!!!
    newNode->next = pre->next;
    pre->next = newNode;
    return 0;
}

