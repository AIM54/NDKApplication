//
// Created by Administrator on 2019/9/10.
//

#ifndef NDKAPPLICATION_MAINCONTROL_H
#define NDKAPPLICATION_MAINCONTROL_H

#include <jni.h>

int registerNativeMethod(JNIEnv *pInterface);

void initGL(JNIEnv *env, jobject jobj);

void resizeGL(JNIEnv *jniEnv, jobject jobj, int width, int height);

void step(JNIEnv *jniEnv, jobject jobj);

void setAssertManager(JNIEnv *jniEnv, jobject claz, jobject assertManager);

void testNewObject();

void testDeleteObject();

void initSurfaceGL(JNIEnv *env, jobject jobj, jobject surface);

void resizeSurfaceGL(JNIEnv *jniEnv, jobject jobj, int width, int height);

void stepSurface(JNIEnv *jniEnv, jobject jobj);


#endif //NDKAPPLICATION_MAINCONTROL_H
