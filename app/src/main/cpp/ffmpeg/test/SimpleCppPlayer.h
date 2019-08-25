//
// Created by Administrator on 2019/8/25.
//

#ifndef NDKAPPLICATION_SIMPLECPPPLAYER_H
#define NDKAPPLICATION_SIMPLECPPPLAYER_H

#include <iostream>
#include <jni.h>
#include <list>

extern "C" {
#include <libavutil/frame.h>
};

class SimpleCppPlayer {

public:
    void playVideo(JNIEnv *jniEnv, jobject surfaceView, char *videoUrl);

private:
    std::list<AVFrame> avFrameList;

};


#endif //NDKAPPLICATION_SIMPLECPPPLAYER_H
