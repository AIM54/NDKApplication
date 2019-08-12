//
// Created by Administrator on 2019/8/5.
//

#ifndef NDKAPPLICATION_MYVIDEOPLAYER_H
#define NDKAPPLICATION_MYVIDEOPLAYER_H

#include <MyVideoPlayer.h>
#include <jni.h>
#include <string>

using std::string;


void JNICALL onPapareForVideo(JNIEnv *env, jobject instance,
             jstring url_);
void playVideo();

#endif //NDKAPPLICATION_MYVIDEOPLAYER_H
