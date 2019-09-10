//
// Created by Administrator on 2019/9/10.
//

#ifndef NDKAPPLICATION_NEWSURFACEVIEWPLAYER_H
#define NDKAPPLICATION_NEWSURFACEVIEWPLAYER_H

#include <jni.h>
#include "VideoDisplayer.h"

class NewSurfaceViewPlayer : public VideoDisplayer {

private:
    jobject surfaceView;
    JNIEnv *jniEnv;
protected:
    AVCodecContext *videoCodecContext;
public:
    NewSurfaceViewPlayer(JNIEnv *env, jobject jobj);

    int setForVideo(AVCodecContext *avCodecContext) override;

    int playVideoFrame(AVFrame *videoFrame) override;


};


#endif //NDKAPPLICATION_NEWSURFACEVIEWPLAYER_H
