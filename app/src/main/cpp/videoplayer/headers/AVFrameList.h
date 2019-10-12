//
// Created by Administrator on 2019/9/23.
//

#ifndef NDKAPPLICATION_AVFRAMELIST_H
#define NDKAPPLICATION_AVFRAMELIST_H
extern "C"{
#include "libavcodec/avcodec.h"
};

class AVFrameList {
private:
    int initSize;
    AVFrame* avFrameArray;

public:
    AVFrameList(int initSize);
    virtual ~AVFrameList();
};


#endif //NDKAPPLICATION_AVFRAMELIST_H
