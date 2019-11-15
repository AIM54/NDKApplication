//
// Created by Administrator on 2019/9/5.
//

#include "VideoAudioPlayer.h"
#include "GlobalConfig.h"
#include <android/native_window_jni.h>

extern "C" {
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/time.h"
};
using namespace std;

int VideoAudioPlayer::playVideo(char *video, JNIEnv *pEnv, jobject surfaceView) {
    this->pEnv = pEnv;
    this->videoUrl = video;
    this->aNativeWindow = ANativeWindow_fromSurface(pEnv, surfaceView);
    isPlaying.store(true);
    decodeThread = new Thread(this);
    decodeThread->start();
    return 0;
}

void VideoAudioPlayer::runningTask() {
    readStreamInfor();
}

int VideoAudioPlayer::readStreamInfor() {
    int status = openIntput(videoUrl);
    if (status < 0) {
        ALOGI("failed to openInput");
        return status;
    }
    status = initDecoder(videoIndex);
    if (status < 0) {
        return status;
    }
    status = initDecoder(audioIndex);
    if (status < 0) {
        return status;
    }
    AVPacket *avPacket = av_packet_alloc();
    if (!avPacket) {
        return -1;
    }
    isPlaying.store(true);
    while ((av_read_frame(avFormatContext, avPacket) >= 0) && isPlaying.load()) {
        double timeStamp =
                avPacket->pts * av_q2d(avFormatContext->streams[videoIndex]->time_base);
        if (avPacket->stream_index == videoIndex) {
        } else if (avPacket->stream_index == audioIndex) {
            ALOGI("audio current pts: %f", timeStamp);
        }
        av_packet_unref(avPacket);
    }

    av_packet_free(&avPacket);
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
    return 0;
}

int VideoAudioPlayer::initDecoder(int streamIndex) {

    double videoLength = avFormatContext->streams[streamIndex]->duration *
                         av_q2d(avFormatContext->streams[streamIndex]->time_base);
    AVCodec *avCodec = avcodec_find_decoder(
            avFormatContext->streams[streamIndex]->codecpar->codec_id);
    if (!avCodec) {
        return -1;
    }
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    if (!avCodecContext) {
        return -1;
    }

    int decodeStatus = avcodec_parameters_to_context(avCodecContext,
                                                     avFormatContext->streams[streamIndex]->codecpar);
    if (decodeStatus < 0) {
        return decodeStatus;
    }
    av_codec_set_pkt_timebase(avCodecContext, avFormatContext->streams[streamIndex]->time_base);
    if ((decodeStatus = avcodec_open2(avCodecContext, avCodec, NULL)) < 0) {
        return decodeStatus;
    }
    if (streamIndex == videoIndex) {
        videoDecoder = new VideoDecoder(avCodec, avCodecContext);
    } else if (streamIndex == audioIndex) {
        audioDecoder = new AudioDecoder(avCodec, avCodecContext);
    }
    return 1;
}


VideoAudioPlayer::~VideoAudioPlayer() {
    ALOGI("before delete decode thread");
    if (decodeThread) {
        ALOGI("before  decodeVideoThread exit");
        decodeThread->exit();
        ALOGI("after  decodeVideoThread exit");
        delete decodeThread;
        decodeThread = nullptr;
    }
    if (videoDecoder) {
        delete videoDecoder;
        videoDecoder = nullptr;
        ALOGI("deleteVideoCoder");
    }
    if (audioDecoder) {
        delete audioDecoder;
        audioDecoder = nullptr;
        ALOGI("deleteAudioCoder");
    }
    if (avFormatContext) {
        avformat_close_input(&avFormatContext);
        avFormatContext = nullptr;
        ALOGI("avformat_close_input");
    }
}


int VideoAudioPlayer::stopPlay() {
    isPlaying.store(false);
    return 0;
}