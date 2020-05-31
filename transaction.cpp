#include "include/transaction.h"


using namespace std;

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
        GLOBAL_CLOCK.fetch_add(1);
        // TODO: a series validation
        for (auto it = writeSet.begin(); it != writeSet.end();){
            auto lo = it->second.ptr;
            if(!lo->validate(*this)) {return false;}
            lo->unlock();
            writeSet.erase(it++);
        }

        for (auto it = readSet.begin(); it != readSet.end(); ){
            auto lo = it->second.ptr;
            if(!lo->validate(*this)) {return false;}
            lo->unlock();
            readSet.erase(it++);
        }


        status = Status::COMMITED;
        return true;
    }
    return false;
}

bool Transaction::abort(){
	if(status == Status::ACTIVE) {
        for (auto it = writeSet.begin(); it != writeSet.end(); it++){
            auto lo = it->second.ptr;
            lo->unlock();
        }

        for (auto it = readSet.begin(); it != readSet.end(); it++){
            auto lo = it->second.ptr;
            lo->unlock();
        }

        readSet.clear();
        writeSet.clear();

	    status = Status::ABORTED;
	}
	return true; // no matter what happen just return true
}

