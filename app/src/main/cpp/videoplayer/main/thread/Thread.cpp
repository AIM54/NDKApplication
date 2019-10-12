//
// Created by Administrator on 2019/9/30.
//

#include <pthread.h>
#include <GlobalConfig.h>
#include "Thread.h"


Thread::Thread() {
    this->mRunnable = nullptr;
}

Thread::Thread(Runnable *runnable) {
    this->mRunnable = runnable;
}

void Thread::start() {
    pthread_create(&pthread, nullptr, run, this);
}

void Thread::exit() {
    pthread_join(pthread, nullptr);
    mRunnable = nullptr;
}

void *Thread::run(void *arg) {
    Thread *decodeThread = (Thread *) arg;
    if (!decodeThread) {
        ALOGI("failed to find decodeThread:");
        return nullptr;
    }
    if (!decodeThread->mRunnable) {
        ALOGI("failed to get Runnable");
        return nullptr;
    }
    decodeThread->mRunnable->runningTask();
    return nullptr;
}