//
// Created by Administrator on 2019/8/5.
//

#include <android/native_window_jni.h>
#include "MyVideoPlayer.h"
#include "GlobalConfig.h"

extern "C" {
#include "VideoPlay.h"
}

#include "NewPlayVideoInterface.h"
#include "SimpleCppPlayer.h"
#include "MutilThreadPlayer.h"
#include "AudioPlayer.h"

char *videoUrl;
std::list<AVFrame *> frameList;

NewPlayVideoInterface *newPlayVideoInterface = nullptr;

AudioPlayer *audioPlayer = nullptr;

extern JavaVM *javaVM;

void JNICALL onPapareForVideo(JNIEnv *env, jobject instance,
                              jstring url_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    if (newPlayVideoInterface) {
        delete newPlayVideoInterface;
    }
    videoUrl = const_cast<char *>(videoPath);
    env->ReleaseStringUTFChars(url_, videoPath);
}

void JNICALL playAudioData(JNIEnv *env, jobject instance,
                           jstring url_, jstring outputUrl_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
    const char *outputAudioPath = env->GetStringUTFChars(outputUrl_, 0);
    if (!audioPlayer) {
        audioPlayer = new AudioPlayer();
    }
    audioPlayer->playAudio(const_cast<char *>(videoPath));
}

void playVideo(JNIEnv *env, jclass type,
               jobject surface) {
    playVideoSimple(env, type, surface);
}


void onDestory() {
    newPlayVideoInterface->stopPlay();
    if (newPlayVideoInterface) {
        delete newPlayVideoInterface;
    }
}

void playVideoSimple(JNIEnv *pEnv, jclass type,
                     jobject surfaceHolder) {
    MutilThreadPlayer *player = new MutilThreadPlayer();
    ALOGE("the video url is:%s", videoUrl);
    player->playVideo(pEnv, surfaceHolder, videoUrl);

}