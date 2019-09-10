//
// Created by Administrator on 2019/9/10.
//

#include "GlobalConfig.h"
#include <android/native_window_jni.h>
#include "NewSurfaceViewPlayer.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
};

NewSurfaceViewPlayer::NewSurfaceViewPlayer(JNIEnv *env, jobject jobj) {
    this->jniEnv = env;
    this->surfaceView = jobj;
}

int NewSurfaceViewPlayer::setForVideo(AVCodecContext *avCodecContext) {
    this->videoCodecContext = avCodecContext;
    rgbFrame = av_frame_alloc();
    int videoWidth = videoCodecContext->width;
    int videoHeight = videoCodecContext->height;
    nativeWindow = ANativeWindow_fromSurface(jniEnv, surfaceView);
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

    swsContenxt = reinterpret_cast<struct SwsContenxt *>(sws_getContext(
            videoCodecContext->width,
            videoCodecContext->height,
            videoCodecContext->pix_fmt,
            videoCodecContext->width, videoCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL));
    return 0;
}

int NewSurfaceViewPlayer::playVideoFrame(AVFrame *avFrame) {
    ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
    //对帧数据进行格式转换，视频的起始高度和结束高度
    int size = sws_scale(reinterpret_cast<SwsContext *>(swsContenxt),
                         (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                         videoCodecContext->height, rgbFrame->data, rgbFrame->linesize);
    ALOGI("size: %d", size);
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
    return 0;
}