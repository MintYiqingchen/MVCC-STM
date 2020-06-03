#include "mvccTransaction.h"
#include <vector>
#include <iostream>
using namespace std;

atomic_long MVCCTransaction::GLOBAL_CLOCK{0};

MVCCTransaction::MVCCTransaction(){
	start_stamp = GLOBAL_CLOCK.load();
	status = Status::ACTIVE;
}
MVCCTransaction::~MVCCTransaction() {
    try {
        if(!commit())
            abort();
    } catch (...) {
        abort();
    }
}
MVCCTransaction::Status MVCCTransaction::getStatus(){
	return status;
}

bool MVCCTransaction::commit(){
    if(status == Status::ACTIVE) {
        
        // get timestamp
        commit_stamp = GLOBAL_CLOCK.fetch_add(1) + 1;
        // check write set
        if(commit_stamp > start_stamp + 1) {
            for(auto& p: writeSet) {
                if(!p.second.ptr->validate(*this))
                    return false;
            }
        }

        //update all read value
        for(auto& p: readSet) {
            p.second.ptr->readCommit(*(p.second.version), start_stamp);
        }

        // update all write value
        for(auto& p: writeSet) {
            p.second.ptr->writeCommit(*(p.second.version));
        }
        status = Status::COMMITTED;
        readSet.clear();
        writeSet.clear();

    }
    return status == Status::COMMITTED;
}

bool MVCCTransaction::abort(){
	if(status == Status::ACTIVE) {
	    status = Status::ABORTED;
        readSet.clear();
        writeSet.clear();
    }
	return true; // no matter what happen just return true
}

