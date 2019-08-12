//
// Created by Administrator on 2019/8/5.
//

#include "MyVideoPlayer.h"
#include "GlobalConfig.h"
#include "NewPlayVideoInterface.h"


NewPlayVideoInterface *newPlayVideoInterface;

SLObjectItf slObjectItf = nullptr;

SLEngineItf slEngineItf = nullptr;

void JNICALL onPapareForVideo(JNIEnv *env, jobject instance,
                              jstring url_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    ALOGI("the path is opened:%s", videoPath);
    if (!newPlayVideoInterface) {
        newPlayVideoInterface = new NewPlayVideoInterface();
    }
    newPlayVideoInterface->openInput(videoPath);
}

void playVideo() {
    ALOGI("playVideo");
    newPlayVideoInterface->playTheVideo();
    slCreateEngine(&slObjectItf,0,0,0,0,0);
    (*slObjectItf)->Realize(slObjectItf,SL_BOOLEAN_TRUE);
    (*slObjectItf)->GetInterface(slObjectItf,SL_IID_ENGINE,&slEngineItf);



}



void onDestory() {
    if (newPlayVideoInterface) {
        delete newPlayVideoInterface;
    }
}