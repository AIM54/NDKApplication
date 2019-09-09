//
// Created by Administrator on 2019/9/6.
//

#ifndef NDKAPPLICATION_VIDEOPLAYCONTROLER_H
#define NDKAPPLICATION_VIDEOPLAYCONTROLER_H

#include <jni.h>

int registerNativeMethod(JNIEnv *pInterface);

int pauseVideo();

int destoryVideo();

int playNewVideo(JNIEnv *env, jclass type, jstring videoUrl_,
              jobject surface);

#endif //NDKAPPLICATION_VIDEOPLAYCONTROLER_H
