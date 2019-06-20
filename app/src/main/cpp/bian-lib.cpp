#include <jni.h>
#include <string>
#include "NdkTest.h"
#include <android/log.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_bian_myapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_bian_myapplication_utils_JavaUtil_getBitMapFromVideo(JNIEnv *env, jobject instance,
        jstring videoPath_) {
    const char *videoPath = env->GetStringUTFChars(videoPath_, 0);

    env->ReleaseStringUTFChars(videoPath_, videoPath);
}