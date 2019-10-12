//
// Created by Administrator on 2019/9/23.
//

#ifndef NDKAPPLICATION_STREAMDECODER_H
#define NDKAPPLICATION_STREAMDECODER_H


extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};

class StreamDecoder {

protected:
    AVCodec *avCodec;
    AVCodecContext* avCodecContext;

public:
    StreamDecoder(AVCodec *codec,AVCodecContext* codecContext);

    virtual ~StreamDecoder();

};


#endif //NDKAPPLICATION_STREAMDECODER_H
