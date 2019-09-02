//
// Created by Administrator on 2019/8/25.
//

#include "SimpleCppPlayer.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C" {
#include <libavformat/avformat.h>
#include <GlobalConfig.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

void SimpleCppPlayer::playVideo(JNIEnv *pEnv, jobject surfaceHolder, char *videoUrl) {
    av_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    int status;
    if ((status = avformat_open_input(&avFormatContext, videoUrl, NULL, NULL)) != 0) {
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        return;
    }
    int videoIndex = -1;
    for (int index = 0; index < avFormatContext->nb_streams; ++index) {
        if (avFormatContext->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }

    double videoLength = avFormatContext->streams[videoIndex]->duration *
                         av_q2d(avFormatContext->streams[videoIndex]->time_base);
    ALOGI("the totalLength of Video is: %f", videoLength);
    AVCodec *avCodec = avcodec_find_decoder(
            avFormatContext->streams[videoIndex]->codecpar->codec_id);

    if (!avCodec) {
        return;
    }
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    if (!avCodecContext) {
        return;
    }
    int decodeStatus = avcodec_parameters_to_context(avCodecContext,
                                                     avFormatContext->streams[videoIndex]->codecpar);
    ALOGI("decodeStatus: %d", decodeStatus);
    if (decodeStatus < 0) {
        return;
    }
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        return;
    }
    AVFrame *avFrame = av_frame_alloc();
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    AVFrame *rgbFrame = av_frame_alloc();
    ALOGI("可以开始解析视频了:");
    int videoWidth = avCodecContext->width;
    int videoHeight = avCodecContext->height;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(pEnv, surfaceHolder);
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
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index == videoIndex) {
            int deocodeResult;
            if ((deocodeResult = avcodec_send_packet(avCodecContext, avPacket)) < 0) {
                break;
            }
            while (deocodeResult >= 0) {
                deocodeResult = avcodec_receive_frame(avCodecContext, avFrame);
                if (decodeStatus < 0) {
                    break;
                }

                double timeStamp =
                        avFrame->pts * av_q2d(avFormatContext->streams[videoIndex]->time_base);
                if (timeStamp > 0) {
                    ALOGI("currentFrameTime: %f", timeStamp);
                }
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
                //对帧数据进行格式转换，视频的起始高度和结束高度
                sws_scale(reinterpret_cast<SwsContext *>(swsContenxt),
                          (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                          avCodecContext->height, rgbFrame->data, rgbFrame->linesize);
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
            }
        }
    }
    av_frame_free(&rgbFrame);
    av_frame_free(&avFrame);
    av_packet_unref(avPacket);
    av_free(bufferArray);
    avcodec_free_context(&avCodecContext);
    avformat_close_input(&avFormatContext);
}