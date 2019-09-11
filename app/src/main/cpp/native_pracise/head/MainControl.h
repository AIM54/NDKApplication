//
// Created by Administrator on 2019/9/10.
//

#ifndef NDKAPPLICATION_MAINCONTROL_H
#define NDKAPPLICATION_MAINCONTROL_H

#include <jni.h>

int registerNativeMethod(JNIEnv *pInterface);

void initGL(JNIEnv *env, jobject jobj);

void resizeGL(JNIEnv *jniEnv, jobject jobj,int width,int height);

void step(JNIEnv *jniEnv, jobject jobj);

void setAssertManager(JNIEnv *jniEnv,jobject claz,jobject assertManager);



#endif //NDKAPPLICATION_MAINCONTROL_H
