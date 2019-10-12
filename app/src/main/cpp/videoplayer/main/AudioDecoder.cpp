//
// Created by Administrator on 2019/9/16.
//

#include "AudioDecoder.h"

AudioDecoder::AudioDecoder(AVCodec *codec, AVCodecContext *codecContext) : StreamDecoder(codec,
                                                                                         codecContext) {

}

AudioDecoder::~AudioDecoder(){
}