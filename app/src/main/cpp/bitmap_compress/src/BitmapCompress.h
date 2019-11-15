//
// Created by Administrator on 2019/11/15.
//

#ifndef NDKAPPLICATION_BITMAPCOMPRESS_H
#define NDKAPPLICATION_BITMAPCOMPRESS_H

#include <jni.h>


int compressBitmap(JNIEnv *env, jclass type, jstring videoUrl_,
                 jobject bitmap);

#endif //NDKAPPLICATION_BITMAPCOMPRESS_H
