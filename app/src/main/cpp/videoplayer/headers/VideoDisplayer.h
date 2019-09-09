//
// Created by Administrator on 2019/9/6.
//

#ifndef NDKAPPLICATION_VIDEODISPLAYER_H
#define NDKAPPLICATION_VIDEODISPLAYER_H
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};

class VideoDisplayer {
public:
    virtual int setForVideo(AVCodecContext *avCodecContext) = 0;

    virtual int playFrame(AVFrame *videoFrame) = 0;

    virtual ~VideoDisplayer() {
    };
};


#endif //NDKAPPLICATION_VIDEODISPLAYER_H
