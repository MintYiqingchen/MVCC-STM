#include "transaction.h"


using namespace std;


Transaction::Transaction(){
	status.store(ACTIVE);
}

Transaction::Transaction(Status myStatus){
	status.store(myStatus);
}

Status Transaction::getStatus(){
	return status.load();
}

bool Transaction::commit(){
	Status expected = ACTIVE;
	status.compare_exchange_strong(&expected,COMMITED);
}

bool Transaction::abort(){
	Status expected = ACTIVE;
	status.compare_exchange_strong(&expected,ABORTED);
}

static Transaction* getLocal(){
	return local;
}

static void setLocal(Transaction* transaction){
	local = transaction;
}
