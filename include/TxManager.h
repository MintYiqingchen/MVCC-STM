//
// Created by mintyi on 6/3/20.
//

#ifndef STM_TXMANAGER_H
#define STM_TXMANAGER_H
#include <set>
#include <mutex>
class TxManager {
    static std::mutex& getMtx() {
        static std::mutex mtx;
        return mtx;
    }
    static std::set<long>& getActiveStamps() {
        static std::set<long> active_stamps;
        return active_stamps;
    }
public:

    static void add(long stamp) {
        std::lock_guard<std::mutex> lk(getMtx());
        getActiveStamps().insert(stamp);
    }
    static void remove(long stamp) {
        std::lock_guard<std::mutex> lk(getMtx());
        getActiveStamps().erase(stamp);
    }

    static long smallest(){
        std::lock_guard<std::mutex> lk(getMtx());
        if(getActiveStamps().empty()) return -1;
        return *getActiveStamps().begin();
    }
};

#endif //STM_TXMANAGER_H
