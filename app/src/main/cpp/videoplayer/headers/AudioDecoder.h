//
// Created by Administrator on 2019/9/16.
//

#ifndef NDKAPPLICATION_AUDIODECODER_H
#define NDKAPPLICATION_AUDIODECODER_H

#include "StreamDecoder.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};


class AudioDecoder : public StreamDecoder {
public:
    AudioDecoder(AVCodec *codec, AVCodecContext *codecContext);

    virtual ~AudioDecoder();

public:

};


#endif //NDKAPPLICATION_AUDIODECODER_H
