//
// Created by Administrator on 2019/8/6.
//

#include <jni.h>
#include <list>
#include <iostream>
#include <stdio.h>
#include "MainControl.h"
#include "FirstOpenGLDrawer.h"
#include "AndroidLog.h"
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

extern "C" {
JNINativeMethod videoPlayerMethod[] = {
        {"init",        "()V",                                   (void *) initGL},
        {"resize",      "(II)V",                                 (void *) resizeGL},
        {"step",        "()V",                                   (void *) step},
        {"initAsserts", "(Landroid/content/res/AssetManager;)V", (void *) setAssertManager}

};
}

FirstOpenGLDrawer *firstOpenGLDrawer = nullptr;

AAssetManager *g_pAssetManager = nullptr;

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


void initGL(JNIEnv *env, jobject jobj) {
    firstOpenGLDrawer = new FirstOpenGLDrawer();
    firstOpenGLDrawer->setAssertManger(g_pAssetManager);
}

void resizeGL(JNIEnv *jniEnv, jobject jobj, jint width, jint height) {
    ALOGI("resize");
    firstOpenGLDrawer->reize(width, height);
}

void setAssertManager(JNIEnv *jniEnv, jobject claz, jobject assertManager) {
    ALOGI("setAssertManager");
    g_pAssetManager = AAssetManager_fromJava(jniEnv, assertManager);

}

void step(JNIEnv *jniEnv, jobject jobj) {
    ALOGI("step");
    firstOpenGLDrawer->step();
}