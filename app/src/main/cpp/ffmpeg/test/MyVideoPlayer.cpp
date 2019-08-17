//
// Created by Administrator on 2019/8/5.
//

#include "MyVideoPlayer.h"
#include "GlobalConfig.h"
#include "NewPlayVideoInterface.h"


NewPlayVideoInterface *newPlayVideoInterface;

void JNICALL onPapareForVideo(JNIEnv *env, jobject instance,
                              jstring url_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    ALOGI("the path is opened:%s", videoPath);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
    }
    newPlayVideoInterface->openInput(videoPath);
}

void JNICALL playAudioData(JNIEnv *env, jobject instance,
                           jstring url_,jstring outputUrl_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    const char *outputAudioPath=env->GetStringUTFChars(outputUrl_,0);
    ALOGI("the path is openeddada:%s\n the outputPaht:%s", videoPath,outputAudioPath);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
    }
    newPlayVideoInterface->playAudio(videoPath);
}

void playVideo() {
    newPlayVideoInterface->playTheVideo();
}


void onDestory() {
    if (newPlayVideoInterface) {
        delete newPlayVideoInterface;
    }
}