//
// Created by mintyi on 5/30/20.
//
#include <thread>
#include "transaction.h"
#include "lockObject.h"

using namespace std;
LockObject A{500}, B{500}, C{500};
constexpr int CHANGE = 200;

void worker() {
    Transaction transaction; // start a transaction
    int a, b, c;
    // a -> b -> c -> a
    if(A.read(transaction, a) != 0 || B.read(transaction, b) != 0) {
        return; // auto abort
    }
    if(a >= CHANGE){
        if(B.write(transaction, b + CHANGE) || A.write(transaction, a - CHANGE))
            return; // auto abort
    } else {
        transaction.abort();
        return;
    }

    if(C.read(transaction, c) != 0 || B.read(transaction, b) != 0) {
        return; // auto abort
    }
    if(b >= CHANGE){
        if(B.write(transaction, b - CHANGE) || C.write(transaction, c + CHANGE))
            return; // auto abort
    } else {
        transaction.abort();
        return;
    }

    if(A.read(transaction, a) != 0 || C.read(transaction, c) != 0) {
        return; // auto abort
    }
    if(c >= CHANGE){
        if(A.write(transaction, a + CHANGE) || C.write(transaction, c - CHANGE))
            return; // auto abort
    } else {
        transaction.abort();
        return;
    }
    transaction.commit();
    // TODO A,B,C status
}
int main(int argc, char** argv) {

}

