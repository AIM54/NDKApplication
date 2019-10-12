//
// Created by Administrator on 2019/9/30.
//

#ifndef NDKAPPLICATION_THREAD_H
#define NDKAPPLICATION_THREAD_H

#include "Runnable.h"

class Thread {
private:
    Runnable *mRunnable;
    pthread_t pthread;

    static void *run(void *arg);

public:
    Thread(Runnable *runnable);

    Thread();

    void start();

    void join();

    void detach();

    void exit();


};


#endif //NDKAPPLICATION_THREAD_H
