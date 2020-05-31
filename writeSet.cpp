#include "WriteSet.h"

WriteSet::WriteSet(){
	map = &local;
}

static WriteSet* WriteSet::getLocal(){
	return new WriteSet();
}

unordered_map<LockObject*,int>::iterator WriteSet::iter(){
	return map->begin();
}

void WriteSet::unlock(){
	for (auto it = map->begin(); it != map->end(); it++){
		it->first->unlock();
	}
}

bool WriteSet::tryLock(long timeout){
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

int WriteSet::get(LockObject* key){
	unordered_map<LockObject*,int>::iterator it;
	it = map->find(key);
	if(it == map->end()) throw "Unexpected get";
	return it->second();
}

void WriteSet::put(LockObject* key, int value){
	map->insert(pair<LockObject*, int>(key, value));
}

void WriteSet::clear(){
	map->clear();
}