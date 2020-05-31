#ifndef _READSET_H_
#define _READSET_H_
#include <unordered_set>

using namespace std;
class LockObject;

class ReadSet{
private:
	ReadSet();

public:
	static ReadSet* getLocal();

	unordered_set<int>::iterator iter();

	void add(int x);

	void clear();

protected:
	thread_local unordered_set<int> local;
	unordered_set<int>* set;
};
#endif