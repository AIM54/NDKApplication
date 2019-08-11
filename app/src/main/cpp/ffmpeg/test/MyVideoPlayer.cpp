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
    ALOGI("the path is opened:%s",videoPath);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
    }
    newPlayVideoInterface->openInput(videoPath);
}

void playVideo() {
    newPlayVideoInterface->playTheVideo();
}

void onDestory() {
    if (newPlayVideoInterface) {
        newPlayVideoInterface->release();
        delete newPlayVideoInterface;
    }
}