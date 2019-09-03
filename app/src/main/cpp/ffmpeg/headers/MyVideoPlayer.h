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

void JNICALL playAudioData(JNIEnv *env, jobject instance,
                           jstring url_, jstring ouptputUrl);

void playVideo(JNIEnv *env, jclass type, jstring videoUrl_,
               jobject surface);


void onDestory();

#endif //NDKAPPLICATION_MYVIDEOPLAYER_H
