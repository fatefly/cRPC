#ifndef CRPC_COMMON_MUTEX_H
#define CRPC_COMMON_MUTEX_H

#include<pthread.h>
#include<memory>
#include"noncopyable.h"

namespace cRPC{

class Spinlock : cRPC::Noncopyable{
public:
    typedef SocpedLockImpl<Spinlock> Lock;

    Spinlock(){
        pthread_spin_init(&m_mutex, 0);
    }

    ~Spinlock(){
        pthread_spin_destroy(&m_mutex);
    }

    void lock(){
        pthread_spin_lock(&m_mutex);
    }

    void unlock(){
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

} //namespace cRPC

#endif //CRPC_COMMON_MUTEX_H