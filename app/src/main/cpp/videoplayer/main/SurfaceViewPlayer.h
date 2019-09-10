//
// Created by Administrator on 2019/9/6.
//

#ifndef NDKAPPLICATION_SURFACEVIEWPLAYER_H
#define NDKAPPLICATION_SURFACEVIEWPLAYER_H

#include "VideoDisplayer.h"
#include <jni.h>
#include <android/native_window.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
};

class SurfaceViewPlayer : public VideoDisplayer {
private:
    jobject mSurfaceView;
    JNIEnv *pENV;
    AVFrame *rgbFrame = nullptr;
    ANativeWindow *nativeWindow = nullptr;
    struct SwsContenxt *swsContext = nullptr;
    AVCodecContext *videoCodecContext = nullptr;
    int videoWidth, videoHeight;

public:
    SurfaceViewPlayer(jobject surfaceView, JNIEnv *env);

    int setForVideo(AVCodecContext *avCodecContext) override;

    int playFrame(AVFrame *videoFrame) override;
    int playFrame(AVFrame *videoFrame, ANativeWindow_Buffer *buffer)
;

    virtual ~SurfaceViewPlayer();
};


#endif //NDKAPPLICATION_SURFACEVIEWPLAYER_H
