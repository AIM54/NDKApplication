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
#include "GlobalConfig.h"

using namespace std;
SLObjectItf slObjectItf = nullptr;

NewPlayVideoInterface::NewPlayVideoInterface() {
    if (!videoDequeue) {
        videoDequeue = new deque<AVFrame>();
    }
}

void NewPlayVideoInterface::openInput(string filePath) {
    av_register_all();
    avformat = avformat_alloc_context();
    if (!avformat) {
        currentPlayState = PlayState::error;
        return;
    }
    int resultCode = avformat_open_input(&avformat, filePath.data(), nullptr, nullptr);
    if (resultCode < 0) {
        currentPlayState = PlayState::error;
        return;
    }
    if (avformat_find_stream_info(avformat, NULL) < 0) {
        currentPlayState = PlayState::error;
        return;
    }
}

void NewPlayVideoInterface::playAudio(std::string url) {
    thread decodeAudioThread(&NewPlayVideoInterface::decodeAudioMethod, this, url);
    decodeAudioThread.detach();
}

/**
 * 解析音频文件取出音频帧的代码
 * @param url
 * @return
 */
int NewPlayVideoInterface::decodeAudioMethod(std::string url) {
    openInput(url);
    if (currentPlayState == PlayState::error) {
        return -1;
    }
    decodeAudioData();
    audioFrame = av_frame_alloc();
    audioPacket = av_packet_alloc();
    av_init_packet(audioPacket);
    int status = 0;
    while ((status = av_read_frame(avformat, audioPacket)) >= 0) {
        if (audioPacket->stream_index == audioStreamIndex) {
            status = avcodec_send_packet(audioCodecContext, audioPacket);
            if (status < 0) {
                ALOGI("failed to send audio packet\n");
                continue;
            }
            while (status >= 0) {
                status = avcodec_receive_frame(audioCodecContext, audioFrame);
                if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) {
                    break;
                } else if (status < 0) {
                    ALOGI("Error during decoding3\n");
                    goto destrory;
                }
                int data_size = av_get_bytes_per_sample(audioCodecContext->sample_fmt);
                if (data_size < 0) {
                    /* This should not occur, checking just for paranoia */
                    ALOGI("Error during decoding4\n");
                    goto destrory;
                }
                double timeStamp =
                        audioFrame->pts * av_q2d(avformat->streams[audioStreamIndex]->time_base);
                ALOGI("currentFrameTime: %f", timeStamp);
            }
        }
    }
    destrory:
    ALOGI("finish decode Audio frame");
    return -1;
}


int NewPlayVideoInterface::decodeAudioData() {
    int status = -1;
    status = av_find_best_stream(avformat, AVMEDIA_TYPE_AUDIO, -1, -1, &audioCodec, 0);
    ALOGI("find Audio Stream");
    if (status < 0) {
        ALOGI("failed to find audio stream");
        return status;
    }
    audioStreamIndex = status;
    if (!audioCodec) {
        status = -1;
        ALOGI("failed to find audioCodec");
        return status;
    }
    audioCodecContext = avcodec_alloc_context3(audioCodec);
    if (!audioCodecContext) {
        status = -1;
        ALOGI("failed to find audioCodecContext");
        return status;
    }
    status = avcodec_parameters_to_context(audioCodecContext,
                                           avformat->streams[audioStreamIndex]->codecpar);
    if (status < 0) {
        return status;
    }
    if ((status = avcodec_open2(audioCodecContext, audioCodec, NULL)) < 0) {
        ALOGI("failed to open audio Stream ");
        return status;
    }
    return 1;
}

int NewPlayVideoInterface::decodeVideoData() {
    int status = -1;
    status = av_find_best_stream(avformat, AVMEDIA_TYPE_VIDEO, -1, -1, &videoCodec, 0);
    if (status < 0) {
        ALOGI("failed to find video stream");
        return status;
    }
    videoStreamIndex = status;
    if (!videoCodec) {
        status = -1;
        ALOGI("failed to find videoCodeer");
        return status;
    }
    videoCodecContext = avcodec_alloc_context3(videoCodec);
    if (!videoCodecContext) {
        status = -1;
        ALOGI("failed to find videoCodecContext");
        return status;
    }
    status = avcodec_parameters_to_context(videoCodecContext,
                                           avformat->streams[videoStreamIndex]->codecpar);
    if (status < 0) {
        return status;
    }
    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
    av_init_packet(avPacket);
    if ((status = avcodec_open2(videoCodecContext, videoCodec, NULL)) < 0) {
        ALOGI("failed to open video Stream ");
        return status;
    }
    int readPacketStatus = 0;
    while ((readPacketStatus = av_read_frame(avformat, avPacket)) >= 0) {
        if (avPacket->stream_index == videoStreamIndex) {
            status = avcodec_send_packet(videoCodecContext, avPacket);
            if (status < 0) {
                ALOGI("Error sending a packet for decoding");
                break;
            }
            while (status >= 0) {
                status = avcodec_receive_frame(videoCodecContext, avFrame);
                if (status == AVERROR(EAGAIN) || status == AVERROR_EOF)
                    break;
                else if (status < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    exit(1);
                }
                double timeStamp =
                        avFrame->pts * av_q2d(avformat->streams[videoStreamIndex]->time_base);
                if (timeStamp > 0) {
                    ALOGI("currentFrameTime: %f", timeStamp);
                }
            }
        }
    }
    ALOGI("readPacketStatus:%d", readPacketStatus);
    return 1;
}


void NewPlayVideoInterface::pauseTheVideo() {
}

void NewPlayVideoInterface::seekToPosition(int position) {

}

void NewPlayVideoInterface::resumePlay() {

}


NewPlayVideoInterface::~NewPlayVideoInterface() {
    if (!avFrame) {
        av_frame_free(&avFrame);
    }
    if (!avPacket) {
        av_packet_free(&avPacket);
    }
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


void NewPlayVideoInterface::playTheVideo() {
    int status = 0;
    if (currentPlayState == PlayState::error) {
        ALOGI("failed to open video File");
        return;
    }
    thread decodeVideoThread(&NewPlayVideoInterface::decodeVideoData, this);
    decodeVideoThread.detach();

}



