//
// Created by Administrator on 2019/8/5.
//

#include <android/native_window_jni.h>
#include "MyVideoPlayer.h"
#include "GlobalConfig.h"
#include "NewPlayVideoInterface.h"


NewPlayVideoInterface *newPlayVideoInterface = nullptr;
extern JavaVM *javaVM;

void JNICALL onPapareForVideo(JNIEnv *env, jobject instance,
                              jstring url_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    ALOGI("the path is opened:%s", videoPath);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
    }
    newPlayVideoInterface->openInput(videoPath);
    env->ReleaseStringUTFChars(url_, videoPath);
}

void JNICALL playAudioData(JNIEnv *env, jobject instance,
                           jstring url_, jstring outputUrl_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    const char *outputAudioPath = env->GetStringUTFChars(outputUrl_, 0);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
        newPlayVideoInterface->setAudioOutputPath(outputAudioPath);
    }
    newPlayVideoInterface->playAudio(videoPath);
    env->ReleaseStringUTFChars(outputUrl_, outputAudioPath);
    env->ReleaseStringUTFChars(url_, videoPath);
}

void playVideo(JNIEnv *env, jclass type,
               jobject surface) {
    newPlayVideoInterface->playTheVideo(env, surface, javaVM);
}


void onDestory() {
    newPlayVideoInterface->stopPlay();
    if (newPlayVideoInterface) {
        delete newPlayVideoInterface;
    }
}