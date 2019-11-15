//
// Created by Administrator on 2019/11/15.
//
#include <setjmp.h>
#include <cstdio>
#include "turbojpeg.h"
#include "jpeglib.h"
#include "BitmapCompress.h"
#include "AndroidLog.h"
#include <android/bitmap.h>

int compressBitmap(JNIEnv *env, jclass type, jstring videoUrl_,
                   jobject bitmap) {
    AndroidBitmapInfo androidBitmapInfo;
    const char *destFilePath = env->GetStringUTFChars(videoUrl_, 0);
    AndroidBitmap_getInfo(env, bitmap, &androidBitmapInfo);
    ALOGI("bitmapInfor imageHeight:%d imageWidth:%d", androidBitmapInfo.height,
          androidBitmapInfo.width);
    env->ReleaseStringUTFChars(videoUrl_,destFilePath);
    return 0;
}

int generateJPEG(int w, int h, jint quality, const char *location, jint quality1) {
    return 1;
}