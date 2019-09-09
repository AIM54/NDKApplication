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

int SurfaceViewPlayer::setForVideo(AVCodecContext *avCodecContext) {
    rgbFrame = av_frame_alloc();
    if (!rgbFrame) {
        ALOGI("failed to alloc avframe");
        return -1;
    }
    videoCodecContext = avCodecContext;
    int videoWidth = videoCodecContext->width;
    int videoHeight = videoCodecContext->height;
    nativeWindow = ANativeWindow_fromSurface(pENV, mSurfaceView);
    if (!nativeWindow) {
        ALOGI("find the native window");
        return -1;
    }
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    //获取缓存每一帧图像所需数组的size
    int imageBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, avCodecContext->width,
                                                   avCodecContext->height, 1);
    //申请图像缓存内存
    uint8_t *bufferArray = static_cast<uint8_t *>(av_malloc(imageBufferSize * sizeof(uint8_t)));
    if (!bufferArray) {
        ALOGI("failed to alloc the buffer array");
        return -1;
    }

    //将缓存数组当中的信息赋值到rgbFrame当中去
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, bufferArray, AV_PIX_FMT_RGBA,
                         avCodecContext->width, avCodecContext->height, 1);

    swsContext = sws_getContext(
            avCodecContext->width,
            avCodecContext->height,
            avCodecContext->pix_fmt,
            avCodecContext->width, avCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        ALOGI("failed to alloc swsContext");
        return -1;
    }
    return 0;
}

int SurfaceViewPlayer::playFrame(AVFrame *videoFrame) {
    ALOGI("ANativeWindow_lock");
    ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
    ALOGI("WindowBuffer.stride:%d", windowBuffer.stride);

    //对帧数据进行格式转换，视频的起始高度和结束高度
    sws_scale(swsContext,
              (uint8_t const *const *) videoFrame->data, videoFrame->linesize, 0,
              videoCodecContext->height, rgbFrame->data, rgbFrame->linesize);
    uint8_t *dst = static_cast<uint8_t *>(windowBuffer.bits);
    int dstStride = windowBuffer.stride * 4;
    uint8_t *src = rgbFrame->data[0];
    int srcStride = rgbFrame->linesize[0];
    // 由于window的stride和帧的stride不同,因此需要逐行复制
    int h;
    for (h = 0; h < videoCodecContext->height; h++) {
        memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
    }
    ANativeWindow_unlockAndPost(nativeWindow);
    ALOGI("unlockAndPost");
}

SurfaceViewPlayer::~SurfaceViewPlayer() {
    if (swsContext) {
        sws_freeContext(swsContext);
    }
    if (rgbFrame) {
        av_frame_free(&rgbFrame);
    }
}