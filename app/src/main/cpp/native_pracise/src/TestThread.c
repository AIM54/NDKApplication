//
// Created by Administrator on 2019/7/14.
//
#include <pthread.h>
#include <stdio.h>
#include <jni.h>
#include <malloc.h>
#include <sys/socket.h>
typedef struct pThreadStruct{
   jint id;
   jlong duration;
   jstring name;
};

void testThead();

void testThead() {
}