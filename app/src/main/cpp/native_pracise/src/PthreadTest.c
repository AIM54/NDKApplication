//
// Created by Administrator on 2019/8/6.
//
#include "PthreadTest.h"
#include <pthread.h>
#include "AndroidLog.h"
#include <jni.h>
void *printSimpleMessage(void *data) {
    ALOGI("the first message from pthread");
}

void testSimpleThread() {
    pthread_t firstThread;
    void *(*firstFunction)(void *data) =printSimpleMessage;
    pthread_create(&firstThread, NULL, firstFunction, NULL);
    pthread_join(firstThread, NULL);
}