//
// Created by Administrator on 2019/9/6.
//

#include "SurfaceViewPlayer.h"
#include "GlobalConfig.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>


SurfaceViewPlayer::SurfaceViewPlayer(jobject surfaceView, JNIEnv *env) {
    mSurfaceView = surfaceView;
    pENV = env;
}

int SurfaceViewPlayer::setForVideo(AVCodecContext *videoCodecContext) {
    this->videoCodecContext = videoCodecContext;
    videoWidth = videoCodecContext->width;
    videoHeight = videoCodecContext->height;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(pENV, mSurfaceView);
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    //获取缓存每一帧图像所需数组的size
    int imageBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoCodecContext->width,
                                                   videoCodecContext->height, 1);
    //申请图像缓存内存
    uint8_t *bufferArray = static_cast<uint8_t *>(av_malloc(imageBufferSize * sizeof(uint8_t)));

    //将缓存数组当中的信息赋值到rgbFrame当中去
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, bufferArray, AV_PIX_FMT_RGBA,
                         videoCodecContext->width, videoCodecContext->height, 1);

    swsContext = reinterpret_cast<struct SwsContenxt *>(sws_getContext(
            videoCodecContext->width,
            videoCodecContext->height,
            videoCodecContext->pix_fmt,
            videoCodecContext->width, videoCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL));
    rgbFrame = av_frame_alloc();
    return 0;
}

int SurfaceViewPlayer::playFrame(AVFrame *avFrame) {
}

int SurfaceViewPlayer::playFrame(AVFrame *avFrame, ANativeWindow_Buffer *windowBuffer) {
    ANativeWindow_lock(nativeWindow, windowBuffer, 0);
    //对帧数据进行格式转换，视频的起始高度和结束高度
    int size = sws_scale(reinterpret_cast<SwsContext *>(swsContext),
                         (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                         videoCodecContext->height, rgbFrame->data, rgbFrame->linesize);
    ALOGI("size: %d", size);
    uint8_t *dst = static_cast<uint8_t *>(windowBuffer->bits);
    int dstStride = windowBuffer->stride * 4;
    uint8_t *src = rgbFrame->data[0];
    int srcStride = rgbFrame->linesize[0];
    // 由于window的stride和帧的stride不同,因此需要逐行复制
    int h;
    for (h = 0; h < videoHeight; h++) {
        memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
    }
    ANativeWindow_unlockAndPost(nativeWindow);
}

SurfaceViewPlayer::~SurfaceViewPlayer() {
    if (swsContext) {
        sws_freeContext(reinterpret_cast<SwsContext *>(swsContext));
    }
    if (rgbFrame) {
        av_frame_free(&rgbFrame);
    }
}