#include <chrono>

#include "lockObject.h"
#include "transaction.h"

int LockObject::read(Transaction& transaction, int& res){
	lock_guard<mutex> lock(mtx);
	if(write_stamp > transaction.getTimestamp())
	    return -1;
	read_stamp = max(read_stamp, transaction.getTimestamp());
	res = _data;
	return 0;
}

int LockObject::write(Transaction &transaction, int res) {
	lock_guard<mutex> lock(mtx);
	if(write_stamp > transaction.getTimestamp() || read_stamp > transaction.getTimestamp())
	    return -1;
    write_stamp = transaction.getTimestamp();
    _data = res;
	return 0;
}