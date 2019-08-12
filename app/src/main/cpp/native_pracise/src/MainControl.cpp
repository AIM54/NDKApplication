//
// Created by Administrator on 2019/8/6.
//

#include <jni.h>
#include <list>
extern "C"{
#include "PthreadTest.h"

}
#include <iostream>
#include <stdio.h>

extern "C" {
JNINativeMethod videoPlayerMethod[] = {
        {"test", "()V", (void *) testSimpleThread}
};
}

int registerNativeMethod(JNIEnv *pInterface);

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = pInterface->FindClass("com/bian/myapplication/utils/NativePracise");
    if (!jobject) {
        return -1;
    }
    if (pInterface->RegisterNatives(jobject, videoPlayerMethod,
                                    sizeof(videoPlayerMethod) / sizeof(videoPlayerMethod[0])) < 0) {
        return -1;
    }
    return JNI_OK;
}
void  testList(){

}