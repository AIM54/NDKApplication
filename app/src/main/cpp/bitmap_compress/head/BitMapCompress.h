//
// Created by Administrator on 2019/7/4.
//
#include <stdio.h>
#include <jni.h>

#ifndef NDKAPPLICATION_BITMAPCOMPRESS_H
#define NDKAPPLICATION_BITMAPCOMPRESS_H

int compressBitmap(JNIEnv *env, jclass type,
                   jobject bitmap,
                   jstring dstPath_);

#endif //NDKAPPLICATION_BITMAPCOMPRESS_H
