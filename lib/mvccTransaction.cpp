#include "mvccTransaction.h"
#include <vector>
using namespace std;

atomic_long MVCCTransaction::GLOBAL_CLOCK{0};

MVCCTransaction::MVCCTransaction(){
	start_stamp = GLOBAL_CLOCK.fetch_add(1) + 1;
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
        // lock all in writeset
        vector<unique_lock<timed_mutex>> lks; // RAII
        vector<pair<LockableList*, LockableNode*>> predecessors;
        vector<any> values;
        for(auto& p: writeSet) {
            predecessors.emplace_back(p.second.ptr, p.second.ptr->predecessor(start_stamp));
            values.push_back(p.second.localValue);
            lks.emplace_back();
            if(!predecessors.back().second->tryLock(500, start_stamp, lks.back())) {
                return false;
            }
        }
        // TODO:insert new Node behind each predecessor
        for(int i = 0; i < values.size(); ++ i){
            if(!predecessors[i].first->commit(values[i], start_stamp, predecessors[i].second))
                return false;
        }
        status = Status::COMMITTED;
        writeSet.clear();
    }
    return status == Status::COMMITTED;
}

bool MVCCTransaction::abort(){
	if(status == Status::ACTIVE) {
	    status = Status::ABORTED;
        writeSet.clear();
    }
	return true; // no matter what happen just return true
}

