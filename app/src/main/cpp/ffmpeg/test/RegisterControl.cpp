//
// Created by Administrator on 2019/8/11.
//
#include <jni.h>
#include "MyVideoPlayer.h"
#include "GlobalConfig.h"

int registerNativeMethod(JNIEnv *pInterface);

JavaVM *javaVM = nullptr;
extern "C" {
JNINativeMethod firstGlMethod[] = {
        {"onpenVideo", "(Ljava/lang/String;)V",                   (void *) onPapareForVideo},
        {"playAudio",  "(Ljava/lang/String;Ljava/lang/String;)V", (void *) playAudioData},
        {"playVideo",  "(Ljava/lang/String;Landroid/view/Surface;)V",               (void *) playVideo},
        {"destory",    "()V",                                     (void *) onDestory}

};
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    javaVM = vm;
    return JNI_VERSION_1_6;
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = pInterface->FindClass("com/bian/myapplication/video/VideoPlayer");
    if (!jobject) {
        ALOGE("can't find class");
        return -1;
    }
    if (pInterface->RegisterNatives(jobject, firstGlMethod,
                                    sizeof(firstGlMethod) / sizeof(firstGlMethod[0])) < 0) {
        ALOGE("can't Register Method");
        return -1;
    }
    return JNI_OK;
}
