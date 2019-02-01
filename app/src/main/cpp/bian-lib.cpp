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
JNIEXPORT jint JNICALL
Java_com_bian_myapplication_utils_NDKUtil_getEncrypMessage(JNIEnv *env, jclass type,
                                                           jstring message_) {

    const char *message = env->GetStringUTFChars(message_, 0);
    int length=strlen(message);
    env->ReleaseStringUTFChars(message_, message);
    __android_log_print(ANDROID_LOG_INFO, "ndk", "%s", "Hello, World");
    return length;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_bian_myapplication_utils_NDKUtil_compressBitMap(JNIEnv *env, jclass type, jobject bitmap) {

    return bitmap ;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_bian_myapplication_utils_NDKUtil_getAddResult(JNIEnv *env, jclass type, jint a, jint b) {
    return getAddResult(a,b);
}