#include <unordered_map>
#include <stack>

using namespace std;

class WriteSet{
private:
	WriteSet(){
		map = &local;
	}

public:
	static WriteSet* getLocal(){
		return new WriteSet();
	}

	unordered_map<LockObject*,int>::iterator iter(){
		return map->begin();
	}

	void unlock(){
		for (auto it = map->begin(); it != map->end(); it++){
			it->first->unlock();
		}
	}

	bool tryLock(long timeout){
		stack<LockObject*> st;
		for (auto it = map->begin(); it != map->end(); it++){
			if(!it->first->tryLock(timeout)){
				for(LockObject* y:st){
					y->unlock();
				}
				return false;
			}
			st.push(it->first);
		}
		return true;
	}

	int get(LockObject* key){
		unordered_map<LockObject*,int>::iterator it;
		it = map->find(key);
		if(it == map->end()) throw "Unexpected get";
		return it->second();
	}

	void put(LockObject* key, int value){
		map->insert(pair<LockObject*, int>(key, value));
	}

	void clear(){
		map->clear();
	}

protected:
	thread_local unordered_map<LockObject*,int> local;
	unordered_map<LockObject*,int>* map;
}