//
// Created by Administrator on 2019/8/18.
//

#include "AudioFrameDataBean.h"
#include <iostream>
#include <stdio.h>
#include <GlobalConfig.h>

AudioFrameDataBean::AudioFrameDataBean(size_t size, uint8_t *paramData) {
    this->packetSize = size;
    this->data = new uint8_t[size];
    memcpy(data, paramData, size * sizeof(uint8_t));
}

AudioFrameDataBean::AudioFrameDataBean(const AudioFrameDataBean &audioFrameDataBean) : packetSize{
        audioFrameDataBean.packetSize}, data{new uint8_t[audioFrameDataBean.packetSize]} {
    packetSize = audioFrameDataBean.packetSize;
    timeStamp=audioFrameDataBean.timeStamp;
    memcpy(data, audioFrameDataBean.data, audioFrameDataBean.packetSize * sizeof(uint8_t));
}

AudioFrameDataBean &AudioFrameDataBean::operator=(const AudioFrameDataBean &audioFrameDataBean) {
    uint8_t *newData = new uint8_t[audioFrameDataBean.packetSize];
    memcpy(newData, audioFrameDataBean.data, audioFrameDataBean.packetSize * sizeof(uint8_t));
    delete[] data;
    data = newData;
    packetSize = audioFrameDataBean.packetSize;
    timeStamp = audioFrameDataBean.timeStamp;
}

double AudioFrameDataBean::getTimeStamp() {
    return timeStamp;
}

void AudioFrameDataBean::setTimeStamp(double timeStamp) {
    this->timeStamp = timeStamp;
}

AudioFrameDataBean::AudioFrameDataBean() {
    this->data = nullptr;
}

uint8_t *AudioFrameDataBean::getData() {
    return data;
}

size_t AudioFrameDataBean::getSize() {
    return packetSize;
}

AudioFrameDataBean::~AudioFrameDataBean() {
    if (data) {
        delete[]data;
    }
}