//
// Created by Administrator on 2019/9/3.
//

#include "NewVideoPlayer.h"

int NewVideoPlayer::seekToFrame(int senconds, bool allowAround) {
    return 0;
}

int NewVideoPlayer::displayVideo(JNIEnv *jniEnv, jobject surfaceView) {
    MutilThreadPlayer::displayVideo(jniEnv, surfaceView);
}

int NewVideoPlayer::resumeTheVideo() {

    return 0;
}

int NewVideoPlayer::pauseTheVideo() {
    return 0;
}