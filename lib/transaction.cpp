#include "transaction.h"
#include "lockObject.hpp"
atomic_long Transaction::GLOBAL_CLOCK{0};

Transaction::Transaction(){
    start_stamp = GLOBAL_CLOCK.load();
    status = Status::ACTIVE;
}
Transaction::~Transaction() {
    try {
        if(!commit())
            abort();
    } catch (...) {
        abort();
    }
}
Transaction::Status Transaction::getStatus(){
    return status;
}

bool Transaction::commit(){
    if(status == Status::ACTIVE) {
        // lock objects in writeSet
        vector<void*> keys;
        vector<LockHelper> lks; // RAII
        for(auto& p: writeSet) {
            lks.emplace_back();
            if(!(p.second.ptr)->tryLock(500, start_stamp, lks.back())) {
                return false;
            }
            keys.push_back(p.first);
        }
        // get timestamp
        commit_stamp = GLOBAL_CLOCK.fetch_add(1) + 1;
        // check read set
        if(commit_stamp > start_stamp + 1) {
            for(auto& p: readSet) {
                if(!p.second.ptr->validate(*this))
                    return false;
            }
        }
        // update all write value
        for(auto& p: writeSet) {
            p.second.ptr->commit(p.second.localValue, commit_stamp);
        }
        status = Status::COMMITTED;
        readSet.clear();
        writeSet.clear();

    }
    return status == Status::COMMITTED;
}

bool Transaction::abort(){
    if(status == Status::ACTIVE) {
        status = Status::ABORTED;
        readSet.clear();
        writeSet.clear();
    }
    return true; // no matter what happen just return true
}
