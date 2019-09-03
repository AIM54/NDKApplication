//
// Created by Administrator on 2019/8/5.
//

#include <android/native_window_jni.h>
#include "MyVideoPlayer.h"
#include "GlobalConfig.h"

extern "C" {
#include "VideoPlay.h"
}

#include "SimpleCppPlayer.h"
#include "AudioPlayer.h"
#include "NewVideoPlayer.h"

AudioPlayer *audioPlayer = nullptr;
extern JavaVM *javaVM;

void JNICALL onPapareForVideo(JNIEnv *env, jobject instance,
                              jstring url_) {
    const char *videoPath = env->GetStringUTFChars(url_, 0);
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

void playVideo(JNIEnv *env, jclass type, jstring videoUrl_,
               jobject surface) {
    const char *videoPath = env->GetStringUTFChars(videoUrl_, 0);
    ALOGI("playVideo in native method:%s", videoPath);
    NewVideoPlayer *newPlayer = new NewVideoPlayer();
    newPlayer->playVideo(env, surface, const_cast<char *>(videoPath));
}
void onDestory(){

}