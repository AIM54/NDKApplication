//
// Created by Administrator on 2019/6/20.
//
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <GlobalConfig.h>

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_testSoLibrary(JNIEnv *env, jclass type) {
    jclass jcls = (*env)->FindClass(env, "com/bian/myapplication/utils/VideoUtil");
    jmethodID jMethodId = (*env)->GetStaticMethodID(env, jcls, "log", "(Ljava/lang/String;)V");
    char *message = "tomcat";
    jstring jMsg = (*env)->NewStringUTF(env, message);
    (*env)->CallStaticVoidMethod(env, jcls, jMethodId, jMsg);

}

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_decodeVideo(JNIEnv *env, jclass type,
                                                        jstring filePath_) {
    const char *filePath = (*env)->GetStringUTFChars(env, filePath_, 0);
    __android_log_print(ANDROID_LOG_INFO, LOG_MARK, "string From Java To C : %s", filePath);
    (*env)->ReleaseStringUTFChars(env, filePath_, filePath);
}