#include "readSet.h"

ReadSet::ReadSet(){
	set = &local;
}

ReadSet* ReadSet::getLocal(){
	return new ReadSet();
}

unordered_set<int>::iterator ReadSet::iter(){
	return set->begin();
}

void ReadSet::add(int x){
	set->insert(x);
}

void ReadSet::clear(){
	set->clear();
}
