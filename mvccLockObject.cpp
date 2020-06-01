#include <chrono>
#include <unordered_map>

#include "mvccLockObject.h"
#include "mvccTransaction.h"

int MVCCLockObject::read(MVCCTransaction& transaction, int& res){
	auto& readSet = transaction.getReadSet();
	auto& writeSet = transaction.getWriteSet();
    if(writeSet.count((void*)this) > 0) { // read local version
        res = writeSet[(void*)this].version->_data;
        return 0;
    }
    auto ptr = head->next;
    auto prev = head;
    while(ptr != nullptr){ //Do we need to add lock here?
        if(ptr->_write_stamp > transaction.getTimestamp()){break;}
        prev = ptr;
        ptr = ptr->next;
    }

    
    res = prev->_data; 

    readSet[(void*)this] = {this,(Version*)prev};
    return 0;
}

int MVCCLockObject::write(MVCCTransaction &transaction, int res) {
	auto& writeSet = transaction.getWriteSet();
    if(writeSet.count((void*)this) > 0) { // set local version
        writeSet[(void*)this].version->_data = res;
        return 0;
    }

    Version* newVersion = new Version(res,transaction.getTimestamp(),transaction.getTimestamp());

    writeSet[(void*)this] = {this,(Version*)newVersion};
    return 0;
}


void MVCCLockObject::readCommit(Version &version, long new_stamp) {
    if(version._read_stamp >= new_stamp) return; //No need to update the read_stamp;
    version.lck.lock();
    if(version._read_stamp < new_stamp) version._read_stamp = new_stamp;
    version.lck.unlock();
}

void MVCCLockObject::writeCommit(Version &version) {
    auto ptr = head->next;
    auto prev = head;
    prev->lck.lock();
    while(ptr != nullptr){ 
        if(ptr->_write_stamp > version._write_stamp){break;}
        ptr->lck.lock();
        prev->lck.unlock();
        prev = ptr;
        ptr = ptr->next;
    }
    version.next = ptr;
    prev->next = &version;
    prev->lck.unlock();
    
}

bool MVCCLockObject::validate(MVCCTransaction &t) {
    auto ptr = head->next;
    auto prev = head;
    while(ptr != nullptr){ //Do we need to add lock here?
        if(ptr->_write_stamp > t.getTimestamp()){break;}
        prev = ptr;
        ptr = ptr->next;
    }
    if(prev->_read_stamp > t.getTimestamp()) {return false;}
    return true;
}
