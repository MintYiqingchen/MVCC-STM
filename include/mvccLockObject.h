#ifndef _LOCKOBJECT_H_
#define _LOCKOBJECT_H_
#include <atomic>
#include <mutex>

#include "mvccTransaction.h"

using namespace std;

struct Version{
    long _write_stamp;
    long _read_stamp;
    int _data;
    mutex lck;
    Version* next;

    Version(int x) : _data(x), _write_stamp(-1), _read_stamp(-1) , next(nullptr){}
    Version(int x, long write_stamp, long read_stamp) : _data(x), _write_stamp(write_stamp), _read_stamp(read_stamp) , next(nullptr){}
    Version(int x, long write_stamp, long read_stamp, Version* node) : _data(x), _write_stamp(write_stamp), _read_stamp(read_stamp) , next(node){}
    ~Version();
};


class MVCCTransaction;

class MVCCLockObject{
public:
	MVCCLockObject(int data){head = new Version(data); _val.store(data); _version_stamp = -1;}

	int read(MVCCTransaction&, int&);

	int write(MVCCTransaction&, int);

    void readCommit(Version &version, long new_stamp);

	void writeCommit(Version &version);

    bool validate(MVCCTransaction&);

    int getValue() const {return _val.load();}
    long getStamp() const {return _version_stamp;}
protected:
	Version* head;
    atomic_int _val;
    long _version_stamp;
};



#endif