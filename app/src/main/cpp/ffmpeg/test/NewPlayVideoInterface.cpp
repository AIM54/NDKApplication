//
// Created by Administrator on 2019/7/12.
//

#include "NewPlayVideoInterface.h"
#include <set>
#include <android/window.h>
#include <android/native_window_jni.h>
#include <array>
#include <linux/media.h>
#include <jni.h>
#include <thread>

using namespace std;

void NewPlayVideoInterface::openInput(string filePath) {
    av_register_all();
    avformat = avformat_alloc_context();
    if (!avformat) {
        currentPlayState = PlayState::error;
        return;
    }
    int resultCode = avformat_open_input(&avformat, filePath.data(), nullptr, nullptr);
    if (!resultCode) {
        currentPlayState = PlayState::error;
        return;
    }
    if (avformat_find_stream_info(avformat, NULL) < 0) {
        currentPlayState = PlayState::error;
        return;
    }
}

void NewPlayVideoInterface::playTheVideo() {
    int status = 0;
    if (currentPlayState == PlayState::error) {
        return;
    }
    status = av_find_best_stream(avformat, AVMEDIA_TYPE_VIDEO, -1, -1, &videoCodec, 0);
    if (!status) {
        return;
    }
    videoStreamIndex = status;
    status = av_find_best_stream(avformat, AVMEDIA_TYPE_AUDIO, -1, -1, &audioCodec, 0);
    if (!status) {
        return;
    }
    audioStreamIndex = status;
    videoCodecContext = avcodec_alloc_context3(videoCodec);
    audioCodecContext = avcodec_alloc_context3(audioCodec);
    avcodec_parameters_from_context(videoCodecParameters,videoCodecContext);
    avcodec_parameters_from_context(audioCodecParameters,audioCodecContext);
    avPacket=av_packet_alloc();
    av_init_packet(avPacket);
    if (!avcodec_open2(videoCodecContext,videoCodec,NULL)){
        return;
    }


}

void NewPlayVideoInterface::pauseTheVideo() {
}

void NewPlayVideoInterface::seekToPosition(int position) {

}

void NewPlayVideoInterface::resumePlay() {

}

void NewPlayVideoInterface::release() {
    if (!videoCodecContext) {
        avcodec_free_context(&videoCodecContext);
    }
    if (!audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
    }
    if (!avformat) {
        avformat_free_context(avformat);
        avformat = nullptr;
    }
}

