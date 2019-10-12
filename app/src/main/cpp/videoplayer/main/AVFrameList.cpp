//
// Created by Administrator on 2019/9/23.
//

#include "AVFrameList.h"

AVFrameList::AVFrameList(int initSize) {
    avFrameArray = static_cast<AVFrame *>(malloc(sizeof(AVFrame) * initSize));


}

AVFrameList::~AVFrameList() {
    delete[] avFrameArray;
}