#include <atomic>


using namespace std;


enum Status{ABORTED,ACTIVE,COMMITED};

class Transaction{

public:
	Transaction();

	Transaction(Status myStatus);

	Status getStatus();

	bool commit();

	bool abort();

	static Transaction getLocal();

	static void setLocal(Transaction transaction);


private:
	const atomic<Status> status;

public:
	static const Transaction *_COMMITED = new Transaction(COMMITED);
	thread_local local;

}