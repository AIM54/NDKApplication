//
// Created by Administrator on 2019/8/25.
//

#include "MutilThreadPlayer.h"
#include <thread>
#include <GlobalConfig.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

using namespace std;

void MutilThreadPlayer::playVideo(JNIEnv *jniEnv, jobject surfaceView, char *videoUrl) {
    isPlaying.store(true);
    thread decodeVideoThread(&MutilThreadPlayer::deocodeVideo, this, videoUrl);
    decodeVideoThread.detach();
    displayVideo(jniEnv, surfaceView);
}


int MutilThreadPlayer::deocodeVideo(char *videoUrl) {
    if (openInputUrl(videoUrl) < 0) {
        return -1;
    }
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    AVFrame *avFrame = av_frame_alloc();
    int decodeVideoStatus;
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index == videoIndex) {
            if ((decodeVideoStatus = avcodec_send_packet(avCodecContext, avPacket)) < 0) {
                av_packet_unref(avPacket);
                break;
            }
            while (decodeVideoStatus >= 0) {
                decodeVideoStatus = avcodec_receive_frame(avCodecContext, avFrame);
                if (decodeVideoStatus == AVERROR(EAGAIN) || decodeVideoStatus == AVERROR_EOF) {
                    av_frame_unref(avFrame);
                    continue;
                } else if (decodeVideoStatus < 0) {
                    av_frame_free(&avFrame);
                    av_packet_free(&avPacket);
                    exit(1);
                }
                double timeStamp =
                        avFrame->pts * av_q2d(avFormatContext->streams[videoIndex]->time_base);
                AVFrame *newFrame = av_frame_alloc();
                av_frame_ref(newFrame, avFrame);
                enqueueFrame(newFrame);
                av_frame_unref(avFrame);
                ALOGI("currentFrameTime: %f", timeStamp);
            }
        }
        av_packet_unref(avPacket);
    }
    av_frame_free(&avFrame);
    av_packet_unref(avPacket);
    av_packet_free(&avPacket);
    return 0;
}

int MutilThreadPlayer::openInputUrl(char *videoUrl) {
    unique_lock<mutex> videoInfoLock(videoInforMutex);
    av_register_all();
    avFormatContext = avformat_alloc_context();
    int status;
    if ((status = avformat_open_input(&avFormatContext, videoUrl, NULL, NULL)) != 0) {
        return status;
    }
    if ((status = avformat_find_stream_info(avFormatContext, NULL)) < 0) {
        return status;
    }
    videoIndex = -1;
    for (int index = 0; index < avFormatContext->nb_streams; ++index) {
        if (avFormatContext->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }

    double videoLength = avFormatContext->streams[videoIndex]->duration *
                         av_q2d(avFormatContext->streams[videoIndex]->time_base);
    ALOGI("the totalLength of Video is: %f", videoLength);
    avCodec = avcodec_find_decoder(
            avFormatContext->streams[videoIndex]->codecpar->codec_id);

    if (!avCodec) {
        return -1;
    }
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (!avCodecContext) {
        return -1;
    }
    int decodeStatus = avcodec_parameters_to_context(avCodecContext,
                                                     avFormatContext->streams[videoIndex]->codecpar);
    ALOGI("decodeStatus: %d", decodeStatus);
    if (decodeStatus < 0) {
        return decodeStatus;
    }
    if ((status = avcodec_open2(avCodecContext, avCodec, NULL)) < 0) {
        return status;
    }
    hasLoadVideoInfo.store(true);
    videoInfoLock.unlock();
    videoInforCond.notify_all();
    return 0;
}

int MutilThreadPlayer::displayVideo(JNIEnv *pEnv, jobject surfaceView) {
    AVFrame *rgbFrame = av_frame_alloc();
    unique_lock<mutex> videoInfoLock(videoInforMutex);
    videoInforCond.wait(videoInfoLock, [this] { return this->hasLoadVideoInfo.load(); });
    ALOGI("可以开始解析视频了:");
    int videoWidth = avCodecContext->width;
    int videoHeight = avCodecContext->height;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(pEnv, surfaceView);
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    //获取缓存每一帧图像所需数组的size
    int imageBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, avCodecContext->width,
                                                   avCodecContext->height, 1);
    //申请图像缓存内存
    uint8_t *bufferArray = static_cast<uint8_t *>(av_malloc(imageBufferSize * sizeof(uint8_t)));

    //将缓存数组当中的信息赋值到rgbFrame当中去
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, bufferArray, AV_PIX_FMT_RGBA,
                         avCodecContext->width, avCodecContext->height, 1);

    struct SwsContenxt *swsContenxt = reinterpret_cast<struct SwsContenxt *>(sws_getContext(
            avCodecContext->width,
            avCodecContext->height,
            avCodecContext->pix_fmt,
            avCodecContext->width, avCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL));

    while (isPlaying.load()) {
        unique_lock<mutex> videoQueueLock(videoQueueMutex);
        consumerCond.wait(videoQueueLock, [this] { return !this->avFrameList.empty(); });
        AVFrame *avFrame = avFrameList.front();
        if (!avFrame) {
            continue;
        }
        ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
        //对帧数据进行格式转换，视频的起始高度和结束高度
        int size = sws_scale(reinterpret_cast<SwsContext *>(swsContenxt),
                             (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                             avCodecContext->height, rgbFrame->data, rgbFrame->linesize);
        ALOGI("size: %d",size);
        uint8_t *dst = static_cast<uint8_t *>(windowBuffer.bits);
        int dstStride = windowBuffer.stride * 4;
        uint8_t *src = rgbFrame->data[0];
        int srcStride = rgbFrame->linesize[0];
        // 由于window的stride和帧的stride不同,因此需要逐行复制
        int h;
        for (h = 0; h < videoHeight; h++) {
            memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
        }
        ANativeWindow_unlockAndPost(nativeWindow);
        av_frame_free(&avFrame);
        avFrameList.pop_front();
        videoQueueLock.unlock();
        produceCond.notify_all();
    }
    return 0;
}

void MutilThreadPlayer::enqueueFrame(AVFrame *avFrame) {
    unique_lock<mutex> videoQueueLock(videoQueueMutex);
    produceCond.wait(videoQueueLock, [this] { return this->avFrameList.size() < 10; });
    avFrameList.push_back(avFrame);
    consumerCond.notify_all();
}