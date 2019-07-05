//
// Created by Administrator on 2019/7/4.
//
#include <stdio.h>
#include <BitMapCompress.h>
#include <turbojpeg.h>
#include <android/bitmap.h>
#include <jni.h>
#include <jpeglib.h>
#include <malloc.h>
#include <android/log.h>

char *LOG_MARK = "BML";
#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_MARK, fmt, ##__VA_ARGS__)

#define ALOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN, LOG_MARK, fmt, ##__VA_ARGS__)

int registerNativeMethod(JNIEnv *pInterface);

JNINativeMethod method[] = {
        {"compressBitmap", "(Landroid/graphics/Bitmap;Ljava/lang/String;)I", (void *) compressBitmap}
};

int compressBitmap(JNIEnv *env, jclass type,
                   jobject bitmap,
                   jstring dstPath_) {
    AndroidBitmapInfo *androidBitmapInfo = malloc(sizeof(AndroidBitmapInfo));
    AndroidBitmap_getInfo(env, bitmap, androidBitmapInfo);
    ALOGI("theMapWidth:%d |height:%d the format: %d", androidBitmapInfo->width,
          androidBitmapInfo->height, androidBitmapInfo->format);
    free(androidBitmapInfo);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = (*pInterface)->FindClass(pInterface,
                                              "com/bian/myapplication/utils/BitMapCompressUtil");
    if (!jobject) {
        return -1;
    }
    if ((*pInterface)->RegisterNatives(pInterface, jobject, method,
                                       sizeof(method) / sizeof(method[0])) < 0) {
        return -1;
    }
    return JNI_OK;
}

