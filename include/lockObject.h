#ifndef _LOCKOBJECT_H_
#define _LOCKOBJECT_H_
#include <atomic>
#include <mutex>
#include "transaction.h"

using namespace std;
class LockObject{
public:
	LockObject(int data):_data(data) {};

	int read(Transaction&, int&);

	int write(Transaction&, int);

protected:
	long read_stamp{-1};
	long write_stamp{-1};
	mutex mtx;
	int _data;
};
#endif