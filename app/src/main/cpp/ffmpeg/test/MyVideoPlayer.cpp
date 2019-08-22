//
// Created by Administrator on 2019/8/5.
//

#include "MyVideoPlayer.h"
#include "GlobalConfig.h"
#include "NewPlayVideoInterface.h"


NewPlayVideoInterface *newPlayVideoInterface = nullptr;

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
    ALOGI("the path is openeddada:%s\n the outputPaht:%s", videoPath, outputAudioPath);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
        newPlayVideoInterface->setAudioOutputPath(outputAudioPath);
    }
    newPlayVideoInterface->playAudio(videoPath);
    env->ReleaseStringUTFChars(outputUrl_, outputAudioPath);
    env->ReleaseStringUTFChars(url_, videoPath);
}

void playVideo() {
    newPlayVideoInterface->playTheVideo();
}


void onDestory() {
    newPlayVideoInterface->stopPlay();
    if (newPlayVideoInterface) {
        delete newPlayVideoInterface;
    }
}