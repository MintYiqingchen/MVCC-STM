#include <unordered_map>
#include <stack>


using namespace std;
class LockObject;

class WriteSet{
private:
	WriteSet();

public:
	static WriteSet* getLocal();

	unordered_map<LockObject*,int>::iterator iter();

	void unlock();

	bool tryLock(long timeout);

	int get(LockObject* key);

	void put(LockObject* key, int value);

	void clear();

protected:
	thread_local unordered_map<LockObject*,int> local;
	unordered_map<LockObject*,int>* map;
}