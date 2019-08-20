//
// Created by Administrator on 2019/8/18.
//

#include "AudioFrameDataBean.h"
#include <iostream>
#include <stdio.h>

AudioFrameDataBean::AudioFrameDataBean(size_t size, uint8_t *paramData) {
    this->packetSize = size;
    if (!data) {
        data = new uint8_t[size];
    }
    memcpy(data, paramData, size * sizeof(uint8_t));
}

void AudioFrameDataBean::setTimeStamp(double timeStamp) {
    this->timeStamp = timeStamp;
}

AudioFrameDataBean::AudioFrameDataBean() {

}

AudioFrameDataBean::~AudioFrameDataBean() {
    if (data) {
        delete[]data;
    }
}