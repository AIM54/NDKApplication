//
// Created by Administrator on 2019/9/5.
//

#include "VideoAudioPlayer.h"
#include "GlobalConfig.h"
#include <thread>
#include <android/native_window_jni.h>
#include "NewSurfaceViewPlayer.h"
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
    videoDisplayer=new NewSurfaceViewPlayer(pEnv,surfaceView);
    thread deocdeVideoThread(&VideoAudioPlayer::decodeVideo, this, videourl);
    deocdeVideoThread.detach();
    displayVideo(surfaceView);
    return 0;
}

int VideoAudioPlayer::decodeAudio(char *videoUrl) {
    int status = openIntput(videoUrl);
    if (status < 0) {
        return status;
    }
    status = initCodec(audioIndex, &audioCodec, &audioCodecContex);
    if (status < 0) {
        return status;
    }
    initSwrContext();
    androidPcmPlayer = new AndroidPcmPlayer();
    isGetAudioInfor.store(true);
    audioInforCond.notify_all();
    decodeStream(audioIndex, audioCodecContex);
    return 1;
}

int VideoAudioPlayer::decodeVideo(char *videoUrl) {
    int status = openIntput(videoUrl);
    if (status < 0) {
        return status;
    }
    status = initCodec(videoIndex, &videoCodec, &videoCodecContext);
    if (status < 0) {
        return status;
    }
    isGetStreamInfor.store(true);
    streamInforCond.notify_all();
    decodeStream(videoIndex, videoCodecContext);
    return 1;

}


int VideoAudioPlayer::decodeStream(int index, AVCodecContext *avCodecContext) {
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    AVFrame *avFrame = av_frame_alloc();
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index == index) {
            decodePacket(avCodecContext, avPacket, avFrame);
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
        } else if (avCodecContext == videoCodecContext) {
            decodeVideoFrame(avFrame);
        }
    }
    return status;
}

int VideoAudioPlayer::decodeAudioFrame(AVFrame *audioFrame) {
    if (!audioResampler) {
        return -1;
    }
    unique_lock<mutex> audioListLock(androidPcmPlayer->audioListMutex);
    androidPcmPlayer->produceCondition.wait(audioListLock,
                                            [this] {
                                                return this->androidPcmPlayer->audioFrameList.size() <
                                                       10;
                                            });
    AudioFrameDataBean *dataBean = audioResampler->getDataBean(audioFrame, audioCodecContex,
                                                               audioIndex, avFormatContext);
    this->androidPcmPlayer->audioFrameList.push_back(*dataBean);
    delete dataBean;
    this->androidPcmPlayer->consumerCondition.notify_all();
    return 0;
}

int VideoAudioPlayer::decodeVideoFrame(AVFrame *avFrame) {
    AVFrame *newFrame = av_frame_alloc();
    av_frame_ref(newFrame, avFrame);
    videoDisplayer->pushIntoVideoList(newFrame);
    av_frame_unref(avFrame);
    return 0;
}

int VideoAudioPlayer::displayVideo(jobject surface) {
    unique_lock<mutex> videoInfoLock(streamInforMutex);
    streamInforCond.wait(videoInfoLock, [this] { return this->isGetStreamInfor.load(); });
    ALOGI("可以开始解析视频了:");
    videoDisplayer->setForVideo(videoCodecContext);
    videoDisplayer->playVideo();
    return 1;
}


int VideoAudioPlayer::playSound() {
    unique_lock<mutex> audioInforLock{audioInforMutex};
    audioInforCond.wait(audioInforLock, [this] { return this->isGetAudioInfor.load(); });
    androidPcmPlayer->play();
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
