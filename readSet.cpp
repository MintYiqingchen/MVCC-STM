#include <unordered_set>

using namespace std;

class ReadSet{
private:
	ReadSet(){
		set = &local;
	}

public:
	static ReadSet* getLocal(){
		return new ReadSet();
	}

	unordered_set<int>::iterator iter(){
		return set->begin();
	}

	void add(int x){
		set->insert(x);
	}

	void clear(){
		set->clear();
	}

protected:
	thread_local unordered_set<int> local;
	unordered_set<int>* set;
}