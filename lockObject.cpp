#include <atomic>
#include <mutex>
#include <chrono>

using namespace std;

#define TIMEOUT 1000

class LockObject{
protected:
	volatile long stamp;
	atomic_int version;
	recursive_mutex mtx;
	unique_lock<recursive_mutex> m_lock(mtx);

public:
	LockObject(int init){
		version.store(init);
		stamp = 0;
	}

	int openRead(){

		ReadSet readSet = ReadSet.getLocal();
		switch(Transaction.getLocal().getStatus()) {
			case COMMITED:
				return version.load();
			case ACTIVE:
				WriteSet WriteSet = WriteSet.getLocal();
				if(writeSet.get(this) == nullptr){
					if(m_lock.owns_lock()){
						throw "Abort Exception";
					}
					readSet.add(this);
					return version.load();

				}
				else{
					int scratch = writeSet.get(this);
					return scratch;
				}
			case ABORTED:
				throw "Abort Exception";
			default:
				throw "unexpected transaction state";
		}
	}

	int opernWrite(){
		switch(Transaction.getLocal().getStatus()) {
			case COMMITED:
				return version;
			case ACTIVE:
				WriteSet WriteSet = WriteSet.getLocal();
				int scratch = writeSet.get(this);
				version.store(scratch);
				writeSet.put(this.scratch);
				return scratch;
			case ABORTED:
				throw "Abort Exception";
			default:
				throw "unexpected transaction state";
		}
	}

	bool tryLock(long timeout){
		return lock.try_lock_for(chrono::milliseconds(timeout));
	}

	void unlock(){
		lock.unlock();
	}


	bool validate(){
		Transaction.Status status = Transaction.getLocak().getStatus();
		switch(status){
			case COMMITED:
				return true;
			case ACTIVE:
				bool free = m_lock.owns_lock();
				bool pure = stamp <= VersionClock.getReadStamp();
				return free&pure;
			case ABORTED:
				return false;
			default:
				throw "unexpected transaction state";

		}
	}
}