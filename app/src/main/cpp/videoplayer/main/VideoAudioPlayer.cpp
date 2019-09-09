//
// Created by Administrator on 2019/9/5.
//

#include "VideoAudioPlayer.h"
#include "GlobalConfig.h"
#include <thread>
#include <android/native_window_jni.h>
#include "SurfaceViewPlayer.h"

extern "C" {
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
};


using namespace std;

int VideoAudioPlayer::playVideo(char *videourl, JNIEnv *pEnv, jobject surfaceView) {
    this->pEnv = pEnv;
    isPlaying.store(true);
    thread decodeThread(&VideoAudioPlayer::openIntput, this, videourl);
    decodeThread.detach();
    displayScreen(surfaceView);
    return 0;
}

int VideoAudioPlayer::decodeStream() {
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    AVFrame *avFrame = av_frame_alloc();
    int status = 0;
    while ((status = av_read_frame(avFormatContext, avPacket)) >= 0) {
        if (avPacket->stream_index == videoIndex) {
            status = decodePacket(videoCodecContext, avPacket, avFrame);
        }
        av_packet_unref(avPacket);
    }
    return 0;
}

int VideoAudioPlayer::decodePacket(AVCodecContext *avCodecContext, AVPacket *avPacket,
                                   AVFrame *avFrame) {
    int status = avcodec_send_packet(avCodecContext, avPacket);
    if (status < 0) {
        return status;
    }
    while (status >= 0) {
        status = avcodec_receive_frame(avCodecContext, avFrame);
        if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) {
            av_frame_unref(avFrame);
            continue;
        } else if (status < 0) {
            av_frame_free(&avFrame);
            av_packet_free(&avPacket);
            return -1;
        }
        if (avCodecContext == audioCodecContex) {
            decodeAudioFrame(avFrame);
        } else {
            decodeVideoFrame(avFrame);
        }
    }
    return status;
}

int VideoAudioPlayer::decodeAudioFrame(AVFrame *audioFrame) {
    if (!audioResampler) {
        return -1;
    }
    unique_lock<mutex> audioListLock(audioListMutex);
    audioProduceCond.wait(audioListLock, [this] { return this->audioDataList.size() < 10; });
    AudioFrameDataBean *dataBean = audioResampler->getDataBean(audioFrame, audioCodecContex,
                                                               audioIndex, avFormatContext);
    audioDataList.push_back(*dataBean);
    delete dataBean;
    audioConsumerCond.notify_all();
    return 0;
}

int VideoAudioPlayer::decodeVideoFrame(AVFrame *avFrame) {
    AVFrame *newFrame = av_frame_alloc();
    av_frame_ref(newFrame, avFrame);
    unique_lock<mutex> videoListLock(videoListMutex);
    videoProduceCond.wait(videoListLock, [this] { return this->videoFrameList.size() < 10; });
    videoFrameList.push_back(newFrame);
    videoConsumerCond.notify_all();
    av_frame_unref(avFrame);
    return 0;
}

int VideoAudioPlayer::displayScreen(jobject surface) {
    AVFrame *rgbFrame = av_frame_alloc();
    unique_lock<mutex> videoInfoLock(streamInforMutex);
    streamInforCond.wait(videoInfoLock, [this] { return this->isGetStreamInfor.load(); });
    ALOGI("可以开始解析视频了:");
    int videoWidth = videoCodecContext->width;
    int videoHeight = videoCodecContext->height;
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(pEnv, surface);
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    //获取缓存每一帧图像所需数组的size
    int imageBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, videoCodecContext->width,
                                                   videoCodecContext->height, 1);
    //申请图像缓存内存
    uint8_t *bufferArray = static_cast<uint8_t *>(av_malloc(imageBufferSize * sizeof(uint8_t)));

    //将缓存数组当中的信息赋值到rgbFrame当中去
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, bufferArray, AV_PIX_FMT_RGBA,
                         videoCodecContext->width, videoCodecContext->height, 1);

    struct SwsContenxt *swsContenxt = reinterpret_cast<struct SwsContenxt *>(sws_getContext(
            videoCodecContext->width,
            videoCodecContext->height,
            videoCodecContext->pix_fmt,
            videoCodecContext->width, videoCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR, NULL, NULL, NULL));

    while (isPlaying.load()) {
        unique_lock<mutex> videoQueueLock(videoListMutex);
        videoConsumerCond.wait(videoQueueLock, [this] { return !this->videoFrameList.empty(); });
        AVFrame *avFrame = videoFrameList.front();
        if (!avFrame) {
            continue;
        }
        ANativeWindow_lock(nativeWindow, &windowBuffer, 0);
        //对帧数据进行格式转换，视频的起始高度和结束高度
        int size = sws_scale(reinterpret_cast<SwsContext *>(swsContenxt),
                             (uint8_t const *const *) avFrame->data, avFrame->linesize, 0,
                             videoCodecContext->height, rgbFrame->data, rgbFrame->linesize);
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
        videoFrameList.pop_front();
        videoQueueLock.unlock();
        videoProduceCond.notify_all();
    }
    return 0;
}

int VideoAudioPlayer::playSound() {
    androidPcmPlayer = new AndroidPcmPlayer();
    androidPcmPlayer->play();
    while (isPlaying.load()) {
        unique_lock<mutex> audioListLock(audioListMutex);
        audioConsumerCond.wait(audioListLock, [this] { return !this->audioDataList.empty(); });
        androidPcmPlayer->addAudioFrameData(audioDataList.front());
        audioDataList.pop_front();
        audioProduceCond.notify_all();
    }
    return 0;
}


int VideoAudioPlayer::openIntput(char *videoUrl) {
    av_register_all();
    avFormatContext = avformat_alloc_context();
    int status;
    if ((status = avformat_open_input(&avFormatContext, videoUrl, NULL, NULL)) != 0) {
        return status;
    }
    if ((status = avformat_find_stream_info(avFormatContext, NULL)) < 0) {
        return status;
    }
    for (int index = 0; index < avFormatContext->nb_streams; ++index) {
        if (avFormatContext->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
        }
        if (avFormatContext->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndex = index;
        }
    }
    int initVideoStatus = initCodec(videoIndex, &videoCodec, &videoCodecContext);

//    int initAudioStatus = initCodec(audioIndex, &audioCodec, &audioCodecContex);
    unique_lock<mutex> streamInforLock(streamInforMutex);
    if (initVideoStatus >= 0) {
        isGetStreamInfor.store(true);
        streamInforCond.notify_all();
        streamInforLock.unlock();
        decodeStream();
    }
    return 0;
}


int VideoAudioPlayer::initSwrContext() {
    audioResampler = new AudioResampler();
    return audioResampler->initSwrContex(audioCodecContex);
}

int VideoAudioPlayer::initCodec(int &streamIndex, AVCodec **avCodec,
                                AVCodecContext **avCodecContext) {
    double videoLength = avFormatContext->streams[streamIndex]->duration *
                         av_q2d(avFormatContext->streams[streamIndex]->time_base);
    *avCodec = avcodec_find_decoder(
            avFormatContext->streams[streamIndex]->codecpar->codec_id);
    if (!*avCodec) {
        return -1;
    }
    *avCodecContext = avcodec_alloc_context3(*avCodec);
    if (!*avCodecContext) {
        return -1;
    }

    int decodeStatus = avcodec_parameters_to_context(*avCodecContext,
                                                     avFormatContext->streams[streamIndex]->codecpar);
    if (decodeStatus < 0) {
        return decodeStatus;
    }
    if ((decodeStatus = avcodec_open2(*avCodecContext, *avCodec, NULL)) < 0) {
        return decodeStatus;
    }
    return 1;
}
