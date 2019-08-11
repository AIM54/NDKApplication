//
// Created by Administrator on 2019/8/6.
//
#include "PthreadTest.h"
#include <pthread.h>
#include "AndroidLog.h"
#include <jni.h>
#include <unistd.h>

/**
 * 消费者线程
 */
pthread_t threadConstomer;
/**
 * 生产者线程
 */
pthread_t threadProducter;

pthread_mutex_t constumerMutex;

pthread_mutex_t producterMutext;

pthread_cond_t producterCond;
/**
 * 当前产品数
 */
int productCount;
static totalNumber;

void *printSimpleMessage(void *data) {
    ALOGI("the first message from pthread");
}

void testSimpleThread() {
    pthread_t firstThread;
    void *(*firstFunction)(void *data) =printSimpleMessage;
    pthread_create(&firstThread, NULL, firstFunction, NULL);
    pthread_detach(firstThread);
}

/**
 * 增加产品数量
 * @param data
 * @return
 */
void *increateProductNumber(void *data) {
    while (totalNumber < 1000) {
        pthread_mutex_lock(&producterMutext);
        productCount++;
        sleep(10);
        pthread_mutex_unlock(&producterMutext);
    }
    pthread_exit(NULL);
}

/**
 * 减少产品数量
 * @param data
 * @return
 */
void *decreaseProductNumber(void *data) {
    while (totalNumber < 1000) {
        pthread_mutex_lock(&constumerMutex);
        if (productCount<=0){
            pthread_cond_wait(&producterCond,&producterMutext);
        }
        productCount--;
        pthread_mutex_unlock(&constumerMutex);
    }
    if (productCount < 0) {
        pthread_exit(NULL);
    }
}

void testProducterAndConstumer(int initProductCount) {
    pthread_create(&threadProducter, NULL, increateProductNumber, NULL);
    pthread_create(&threadConstomer, NULL, decreaseProductNumber, NULL);
    pthread_detach(threadConstomer);
    pthread_mutex_init(&producterMutext, NULL);
    pthread_mutex_init(&constumerMutex, NULL);
    pthread_cond_init(&producterCond, NULL);
    pthread_detach(threadConstomer);
    pthread_detach(threadProducter);
}
