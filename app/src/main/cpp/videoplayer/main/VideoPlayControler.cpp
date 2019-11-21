//
// Created by Administrator on 2019/9/6.
//

#include <GlobalConfig.h>
#include <AvInfor.h>
#include "VideoPlayControler.h"
#include "VideoAudioPlayer.h"

VideoAudioPlayer *videoAudioPlayer = nullptr;

JNINativeMethod firstGlMethod[] = {
        {"nPlayVideo",     "(Ljava/lang/String;Landroid/view/Surface;)I", (void *) playNewVideo},
        {"nPauseTheVideo", "()I",                                         (void *) pauseVideo},
        {"nDestroyPlay",   "()I",                                         (void *) destoryVideo}
};


int playNewVideo(JNIEnv *env, jclass type, jstring videoUrl_,
                 jobject surface) {
    char *videoUrl = const_cast<char *>(env->GetStringUTFChars(videoUrl_, 0));
    if (!videoAudioPlayer) {
        videoAudioPlayer = new VideoAudioPlayer();
        videoAudioPlayer->playVideo(videoUrl, env, surface);
    }
    return 0;
}


int pauseVideo() {
    return 0;
}

int destoryVideo() {

    if (videoAudioPlayer) {
        ALOGI("before destory");
        videoAudioPlayer->stopPlay();
        delete videoAudioPlayer;
        videoAudioPlayer = nullptr;
        ALOGI("after destory123");
    }
    return 0;
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

int registerNativeMethod(JNIEnv *pInterface) {
    jclass jobject = pInterface->FindClass("com/bian/myapplication/video/VideoAudioPlayer");
    if (!jobject) {
        ALOGE("can't find class");
        return -1;
    }
    if (pInterface->RegisterNatives(jobject, firstGlMethod,
                                    sizeof(firstGlMethod) / sizeof(firstGlMethod[0])) < 0) {
        ALOGE("can't Register Method");
        return -1;
    }
    return JNI_OK;
}
