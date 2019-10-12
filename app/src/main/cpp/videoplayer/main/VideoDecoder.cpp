//
// Created by Administrator on 2019/9/16.
//

#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(AVCodec *codec, AVCodecContext *codecContext) : StreamDecoder(codec,
                                                                                         codecContext) {

}

VideoDecoder::~VideoDecoder() {

}