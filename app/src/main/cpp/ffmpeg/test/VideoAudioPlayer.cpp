//
// Created by Administrator on 2019/9/5.
//

#include "VideoAudioPlayer.h"
#include "GlobalConfig.h"
#include <thread>
#include <libavutil/opt.h>

extern "C" {
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
};
using namespace std;

int VideoAudioPlayer::playVideo(char *videourl, jobject surfaceView) {
    thread decodeThread(&VideoAudioPlayer::openIntput, this, videourl);
    decodeThread.detach();
    thread playSoundThread(&VideoAudioPlayer::playSound, this);
    playSoundThread.detach();
    displayScreen(surfaceView);
    return 0;
}

int VideoAudioPlayer::decodeStream() {
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    AVFrame *avFrame = av_frame_alloc();
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index == audioIndex) {
            decodePacket(audioCodecContex, avPacket, avFrame);
        } else if (avPacket->stream_index == videoIndex) {
            decodePacket(videoCodecContext, avPacket, avFrame);
        }
        av_frame_unref(avFrame);
        av_packet_unref(avPacket);
    }
    return 0;
}

int VideoAudioPlayer::decodePacket(AVCodecContext *avCodecContext, AVPacket *avPacket,
                                   AVFrame *avFrame) {
    int status = avcodec_send_packet(avCodecContext, avPacket);
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
    return 0;
}

int VideoAudioPlayer::decodeAudioFrame(AVFrame *audioFrame) {
    if (!audioResampler) {
        return -1;
    }
    unique_lock<mutex> audioListLock(audioListMutex);

    AudioFrameDataBean *dataBean = audioResampler->getDataBean(audioFrame, audioCodecContex,
                                                               audioIndex, avFormatContext);


    return 0;
}

int VideoAudioPlayer::decodeVideoFrame(AVFrame *avFrame) {
    return 0;
}

int VideoAudioPlayer::displayScreen(jobject surface) {

}

int VideoAudioPlayer::playSound() {
    androidPcmPlayer = new AndroidPcmPlayer();
    androidPcmPlayer->play();
    while (isPlaying.load()) {
        androidPcmPlayer->addAudioFrameData(getAudioFrameBean());
    }
}

AudioFrameDataBean &VideoAudioPlayer::getAudioFrameBean() {

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
    int initVideoStatus = initCodec(videoIndex, videoCodec, videoCodecContext);

    int initAudioStatus = initCodec(audioIndex, audioCodec, audioCodecContex);
    if (initAudioStatus >= 0 && initVideoStatus >= 0 && initSwrContext() >= 0) {
        decodeStream();
    }
    return 0;
}


int VideoAudioPlayer::initSwrContext() {
    audioResampler = new AudioResampler();
    audioResampler->initSwrContex(audioCodecContex);
}

int VideoAudioPlayer::initCodec(int &streamIndex, AVCodec *avCodec,
                                AVCodecContext *avCodecContext) {
    double videoLength = avFormatContext->streams[streamIndex]->duration *
                         av_q2d(avFormatContext->streams[streamIndex]->time_base);
    ALOGI("the totalLength of Video is: %f", videoLength);
    avCodec = avcodec_find_decoder(
            avFormatContext->streams[streamIndex]->codecpar->codec_id);
    if (!videoCodec) {
        return -1;
    }
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (!avCodecContext) {
        return -1;
    }

    int decodeStatus = avcodec_parameters_to_context(avCodecContext,
                                                     avFormatContext->streams[streamIndex]->codecpar);
    ALOGI("decodeStatus: %d", decodeStatus);
    if (decodeStatus < 0) {
        return decodeStatus;
    }
    if ((decodeStatus = avcodec_open2(avCodecContext, avCodec, NULL)) < 0) {
        return decodeStatus;
    }
    return 1;
}
