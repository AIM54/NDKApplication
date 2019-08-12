//
// Created by Administrator on 2019/7/12.
//

#ifndef NDKAPPLICATION_NEWPLAYVIDEO_H
#define NDKAPPLICATION_NEWPLAYVIDEO_H

#include <string>
#include <pthread.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

enum class PlayState {
    playing, paused, stoped, readyForPlay, error
};

class NewPlayVideoInterface {
public:
    bool isPlayingVideo;
    std::string theVideoUrl;
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

private:
    std::string playerName;
    PlayState currentPlayState;
    AVFormatContext *avformat;
    AVCodec *videoCodec;
    AVCodec *audioCodec;
    AVCodec *subtitleCodeC;
    AVCodecContext* videoCodecContext;
    AVCodecContext* audioCodecContext;
    AVCodecParameters* videoCodecParameters;
    AVCodecParameters*  audioCodecParameters;
    AVPacket *avPacket;
    AVFrame *avFrame;
    int videoStreamIndex;
    int audioStreamIndex;
    int decodeAudioData();
    int decodeVideoData();

};



#endif //NDKAPPLICATION_NEWPLAYVIDEO_H
