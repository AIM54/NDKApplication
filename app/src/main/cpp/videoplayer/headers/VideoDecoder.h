
//
// Created by Administrator on 2019/9/16.
//

#ifndef NDKAPPLICATION_VIDEODECODER_H
#define NDKAPPLICATION_VIDEODECODER_H


#include "StreamDecoder.h"

class VideoDecoder : public StreamDecoder {
public:
    VideoDecoder(AVCodec *codec, AVCodecContext *codecContext);

    virtual ~VideoDecoder() override;

public:

};


#endif //NDKAPPLICATION_VIDEODECODER_H
