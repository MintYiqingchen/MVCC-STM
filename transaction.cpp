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
        status = Status::COMMITED;
        return true;
    }
    return false;
}

bool Transaction::abort(){
	if(status == Status::ACTIVE) {
	    status = Status::ABORTED;
	}
	return true; // no matter what happen just return true
}

