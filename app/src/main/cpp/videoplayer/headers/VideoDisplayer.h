//
// Created by Administrator on 2019/9/6.
//

#ifndef NDKAPPLICATION_VIDEODISPLAYER_H
#define NDKAPPLICATION_VIDEODISPLAYER_H


extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

#include <mutex>
#include <list>
#include <android/native_window.h>

class VideoDisplayer {

protected:
    bool isPlayingVideo{false};
    AVFrame *rgbFrame = nullptr;
    std::list<AVFrame *> videoList;
    std::mutex videoListMutex;
    std::condition_variable videoConsumer;
    std::condition_variable videoProducer;
    ANativeWindow_Buffer windowBuffer;
    ANativeWindow *nativeWindow = nullptr;
    struct SwsContenxt *swsContenxt = nullptr;
public:


    virtual int setForVideo(AVCodecContext *avCodecContext) = 0;

    virtual int playVideo();

    virtual int playVideoFrame(AVFrame *videoFrame) = 0;

    virtual int pushIntoVideoList(AVFrame *avFrame);

    VideoDisplayer() {

    }

    virtual ~VideoDisplayer() {
    };
};


#endif //NDKAPPLICATION_VIDEODISPLAYER_H
