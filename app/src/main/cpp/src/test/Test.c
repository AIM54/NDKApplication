//
// Created by Administrator on 2019/6/20.
//
#include <jni.h>
#include <android/log.h>

JNIEXPORT void JNICALL
Java_com_bian_myapplication_utils_VideoUtil_testSoLibrary(JNIEnv *env, jclass type) {
    jclass jcls = (*env)->FindClass(env, "com/bian/myapplication/utils/VideoUtil");
    jmethodID jMethodId = (*env)->GetStaticMethodID(env, jcls, "log", "(Ljava/lang/String;)V");
    char *message = "tomcat";
    jstring jMsg = (*env)->NewStringUTF(env, message);
    (*env)->CallStaticVoidMethod(env, jcls, jMethodId, jMsg);
}
