//
// Created by Administrator on 2019/7/12.
//

#ifndef NDKAPPLICATION_NEWPLAYVIDEO_H
#define NDKAPPLICATION_NEWPLAYVIDEO_H

#include <string>
#include <pthread.h>
#include <deque>
#include <atomic>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "AudioFrameDataBean.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}

enum class PlayState {
    playing, paused, stoped, readyForPlay, error
};

class NewPlayVideoInterface {
public:
    friend bool isCanPushDataIntoAudioQueue(NewPlayVideoInterface &newPlayVideoInterface);

    friend bool isAudioQueueNoEmpty(NewPlayVideoInterface &newPlayVideoInterface);


    bool isPlayingVideo;
    std::string theVideoUrl;
    char *outputAudioPath;
    std::string outputVideoPath;


    NewPlayVideoInterface();

    virtual ~NewPlayVideoInterface();

    virtual void openInput(std::string);

    virtual void playTheVideo();

    virtual void pauseTheVideo();

    virtual void seekToPosition(int position);

    virtual void resumePlay();

    void playAudio(std::string url);


    void setCurrentPlayState();

    PlayState getCurentPlayState() {
        return currentPlayState;
    }

    void setAudioOutputPath(const char *string);

private:
    std::string playerName;
    PlayState currentPlayState;
    AVFormatContext *avformat;
    AVCodec *videoCodec;
    AVCodec *audioCodec;
    AVCodec *subtitleCodeC;
    AVCodecContext *videoCodecContext;
    AVCodecContext *audioCodecContext;
    AVCodecParameters *videoCodecParameters;
    AVCodecParameters *audioCodecParameters;
    AVPacket *avPacket;
    AVFrame *avFrame;
    AVPacket *audioPacket;
    SwrContext *audioSwrContext;
    AVFrame *audioFrame;
    int videoStreamIndex;
    int audioStreamIndex;

    std::atomic_bool isQuiet{false};
    std::deque<AVFrame> videoDequeue;
    std::deque<AudioFrameDataBean> audioDeque;


    SLObjectItf slObjectItf1 = nullptr;
    SLEngineItf slEngine;
    SLObjectItf outputMixObject;
    //混合输出的辅助效果用于缓冲队列
    const   SLEnvironmentalReverbSettings slEnvironmentalReverbSettings ;
    SLEnvironmentalReverbItf outputMixoutputEnvironmentalReverbIter = nullptr;

    int decodeAudioData();

    int decodeAudioMethod(std::string url);

    int playMusicInAndroid();

    int decodeVideoData();

    int displayVideoOnScreen();


    AVFrame &popAudioFrameFromQueue();


    void setupAndroidEnv();

    void pushToQueue(AudioFrameDataBean *dataBean);

    AudioFrameDataBean &popAudioFrameBean();

};

bool isCanPushDataIntoAudioQueue(NewPlayVideoInterface &newPlayVideoInterface);

bool isAudioQueueNoEmpty(NewPlayVideoInterface &newPlayVideoInterface);

#endif //NDKAPPLICATION_NEWPLAYVIDEO_H
