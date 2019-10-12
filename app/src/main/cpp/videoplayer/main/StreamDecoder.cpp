//
// Created by Administrator on 2019/9/23.
//

#include "StreamDecoder.h"
#include "GlobalConfig.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

StreamDecoder::StreamDecoder(AVCodec *codec, AVCodecContext *codecContext) {
    this->avCodecContext = codecContext;
    this->avCodec = codec;
    ALOGI("StreamDecoder");
}

StreamDecoder::~StreamDecoder() {
    if (avCodecContext) {
        avcodec_free_context(&avCodecContext);
        avCodecContext = nullptr;
        avCodec = nullptr;
        ALOGI("Delete StreamDecoder");
    }
}