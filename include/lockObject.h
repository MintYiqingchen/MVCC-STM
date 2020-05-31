#include <atomic>
#include <mutex>

#include "readSet.h"
#include "writeSet.h"

using namespace std;

#define TIMEOUT 1000

class LockObject{


public:
	LockObject(int init);

	int openRead();

	int opernWrite();

	bool tryLock(long timeout);

	void unlock();

	bool validate();

protected:
	volatile long stamp;
	atomic_int version;
	recursive_mutex mtx;
	unique_lock<recursive_mutex> m_lock(mtx);
}